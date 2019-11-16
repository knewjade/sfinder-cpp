#ifndef FINDER_PERFECT_HPP
#define FINDER_PERFECT_HPP

#include "types.hpp"
#include "spins.hpp"

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    namespace {
        bool validate(const core::Field &field, int maxLine) {
            int sum = maxLine - field.getBlockOnX(0, maxLine);
            for (int x = 1; x < core::FIELD_WIDTH; x++) {
                int emptyCountInColumn = maxLine - field.getBlockOnX(x, maxLine);
                if (field.isWallBetween(x, maxLine)) {
                    if (sum % 4 != 0)
                        return false;
                    sum = emptyCountInColumn;
                } else {
                    sum += emptyCountInColumn;
                }
            }

            return sum % 4 == 0;
        }
    }

    enum SearchTypes {
        TSpin = 1,
    };

    template<class M>
    class Mover;

    template<class C>
    class RecordComparator;

    template<class M = core::srs::MoveGenerator, class C = Candidate>
    class PerfectClearFinder {
    public:
        PerfectClearFinder<M, C>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : mover(Mover<M>(factory, moveGenerator, reachable)), recordComparator(RecordComparator<C>()) {
        }

        Solution run(const Configure &configure, const C &candidate, Solution &solution) {
            // Create current record & best record
            best = Record{
                    std::vector(solution),
                    INT_MAX,
                    INT_MAX,
                    INT_MAX,
                    0,
            };

            // Execute
            search(configure, candidate, solution);

            return best.solution[0].x == -1 ? kNoSolution : std::vector<Operation>(best.solution);
        }

        void search(const Configure &configure, const C &candidate, Solution &solution) {
            if (recordComparator.isWorseThanBest(configure.leastLineClears, best, candidate)) {
                return;
            }

            auto depth = candidate.depth;

            auto &pieces = configure.pieces;
            auto &moves = configure.movePool[depth];

            auto currentIndex = candidate.currentIndex;
            assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
            auto holdIndex = candidate.holdIndex;
            assert(-1 <= holdIndex && holdIndex < configure.pieceSize);

            auto holdCount = candidate.holdCount;

            bool canUseCurrent = currentIndex < configure.pieceSize;
            if (canUseCurrent) {
                assert(currentIndex < pieces.size());
                auto &current = pieces[currentIndex];

                moves.clear();
                mover.move(
                        configure, candidate, solution, moves, current, currentIndex + 1, holdIndex, holdCount, this
                );
            }

            if (0 <= holdIndex) {
                assert(holdIndex < pieces.size());

                // Hold exists
                if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                    auto &hold = pieces[holdIndex];

                    moves.clear();
                    mover.move(
                            configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex,
                            holdCount + 1, this
                    );
                }
            } else {
                assert(canUseCurrent);

                // Empty hold
                int nextIndex = currentIndex + 1;
                assert(nextIndex < pieces.size() + 1);

                if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                    assert(nextIndex < pieces.size());
                    auto &next = pieces[nextIndex];

                    moves.clear();
                    mover.move(
                            configure, candidate, solution, moves, next, nextIndex + 1, currentIndex,
                            holdCount + 1, this
                    );
                }
            }
        }

        void accept(const Configure &configure, const Record &record) {
            assert(!best.solution.empty());

            if (best.solution[0].x == -1 || recordComparator.shouldUpdate(configure.leastLineClears, best, record)) {
                best = Record(record);
            }
        }

    private:
        Mover<M> mover;
        RecordComparator<C> recordComparator;
        Record best;
    };

    template<class M>
    class Mover {
    public:
        Mover<M>(const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable)
                : factory(factory), moveGenerator(moveGenerator), reachable(reachable) {
        }

        void move(
                const Configure &configure,
                const Candidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount,
                PerfectClearFinder<M, Candidate> *finder
        ) {
            assert(0 < candidate.leftLine);

            auto nextLeftNumOfT = pieceType == core::PieceType::T ? candidate.leftNumOfT - 1 : candidate.leftNumOfT;

            moveGenerator.search(moves, candidate.field, pieceType, candidate.leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(pieceType, move.rotateType);

                auto freeze = core::Field(candidate.field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                solution[candidate.depth].pieceType = pieceType;
                solution[candidate.depth].rotateType = move.rotateType;
                solution[candidate.depth].x = move.x;
                solution[candidate.depth].y = move.y;

                int tSpinAttack = getAttackIfTSpin(
                        reachable, factory, candidate.field, pieceType, move, numCleared, candidate.b2b
                );

                int nextSoftdropCount = move.harddrop ? candidate.softdropCount : candidate.softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? candidate.lineClearCount + 1 : candidate.lineClearCount;
                int nextCurrentCombo = 0 < numCleared ? candidate.currentCombo + 1 : 0;
                int nextMaxCombo = candidate.maxCombo < nextCurrentCombo ? nextCurrentCombo : candidate.maxCombo;
                int nextTSpinAttack = candidate.tSpinAttack + tSpinAttack;
                bool nextB2b = 0 < numCleared ? (tSpinAttack != 0 || numCleared == 4) : candidate.b2b;

                int nextLeftLine = candidate.leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto record = Record{
                            solution, nextSoftdropCount, nextHoldCount, nextLineClearCount, nextMaxCombo,
                            nextTSpinAttack
                    };
                    finder->accept(configure, record);
                    return;
                }

                auto nextDepth = candidate.depth + 1;
                if (configure.maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                auto nextCandidate = Candidate{
                        freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                        nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                        nextTSpinAttack, nextB2b, nextLeftNumOfT,
                };
                finder->search(configure, nextCandidate, solution);
            }
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };

    template<class C>
    class RecordComparator {
    public:
        bool isWorseThanBest(bool leastLineClears, const Record &best, const C &current);

        bool shouldUpdate(bool leastLineClears, const Record &oldRecord, const Record &newRecord);
    };

    template<>
    inline bool RecordComparator<Candidate>::isWorseThanBest(
            bool leastLineClears, const Record &best, const Candidate &current
    ) {
        if (current.leftNumOfT == 0) {
            if (current.tSpinAttack != best.tSpinAttack) {
                return current.tSpinAttack < best.tSpinAttack;
            }

            return best.softdropCount < current.softdropCount;
        }

        return false;
    }

    inline bool shouldUpdateLeastLineClear(
            const Record &oldRecord, const Record &newRecord
    ) {
        if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
            return oldRecord.tSpinAttack < newRecord.tSpinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return newRecord.lineClearCount < oldRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    inline bool shouldUpdateMostLineClear(
            const Record &oldRecord, const Record &newRecord
    ) {
        if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
            return oldRecord.tSpinAttack < newRecord.tSpinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.maxCombo != oldRecord.maxCombo) {
            return oldRecord.maxCombo < newRecord.maxCombo;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return oldRecord.lineClearCount < newRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    template<>
    inline bool RecordComparator<Candidate>::shouldUpdate(
            bool leastLineClears, const Record &oldRecord, const Record &newRecord
    ) {
        if (leastLineClears) {
            return shouldUpdateLeastLineClear(oldRecord, newRecord);
        } else {
            return shouldUpdateMostLineClear(oldRecord, newRecord);
        }
    }

    template<class T = core::srs::MoveGenerator>
    class PerfectFinder {
    public:
        PerfectFinder<T>(const core::Factory &factory, T &moveGenerator)
                : factory(factory), moveGenerator(moveGenerator), reachable(core::srs_rotate_end::Reachable(factory)) {
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty, bool leastLineClears, int initCombo, SearchTypes searchTypes
        ) {
            assert(1 <= maxDepth);

            // Copy field
            auto freeze = core::Field(field);

            // Initialize moves
            std::vector<std::vector<core::Move>> movePool(maxDepth);
            for (int index = 0; index < maxDepth; ++index) {
                movePool[index] = std::vector<core::Move>{};
            }

            // Initialize solution
            Solution solution(maxDepth);
            for (int index = 0; index < maxDepth; ++index) {
                solution[index] = Operation{
                        core::PieceType::T, core::RotateType::Spawn, -1, -1
                };
            }

            // Initialize configure
            const Configure configure{
                    pieces,
                    movePool,
                    maxDepth,
                    static_cast<int>(pieces.size()),
                    leastLineClears,
            };

            // Count up T
            int leftNumOfT = std::count(pieces.begin(), pieces.end(), core::PieceType::T);

            // Create candidate
            Candidate candidate = holdEmpty
                                  ? Candidate{freeze, 0, -1, maxLine, 0, 0, 0, 0,
                                              initCombo, initCombo, 0, true, leftNumOfT}
                                  : Candidate{freeze, 1, 0, maxLine, 0, 0, 0, 0,
                                              initCombo, initCombo, 0, true, leftNumOfT};

            auto pcf = PerfectClearFinder(factory, moveGenerator, reachable);
            return pcf.run(configure, candidate, solution);
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        ) {
            return run(field, pieces, maxDepth, maxLine, holdEmpty, true, 0, SearchTypes::TSpin);
        }

    private:
        const core::Factory &factory;
        T &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };
}

#endif //FINDER_PERFECT_HPP
