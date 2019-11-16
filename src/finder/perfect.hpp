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

    template<class M, class C>
    class Mover;

    template<class C, class R>
    class Recorder;

    template<class M = core::srs::MoveGenerator, class C = TSpinCandidate, class R = TSpinRecord>
    class PerfectClearFinder {
    public:
        PerfectClearFinder<M, C, R>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : mover(Mover<M, C>(factory, moveGenerator, reachable)), recorder(Recorder<C, R>()) {
        }

        Solution run(const Configure &configure, const C &candidate) {
            recorder.clear();

            // Initialize solution
            Solution solution(configure.maxDepth);
            std::fill(solution.begin(), solution.end(), Operation{
                    core::PieceType::T, core::RotateType::Spawn, -1, -1
            });

            // Execute
            search(configure, candidate, solution);

            auto &best = recorder.best();
            return best.solution.empty() ? kNoSolution : std::vector<Operation>(best.solution);
        }

        void search(const Configure &configure, const C &candidate, Solution &solution) {
            if (recorder.isWorseThanBest(configure.leastLineClears, candidate)) {
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

        void accept(const Configure &configure, const Solution &solution, const TSpinCandidate &current) {
            if (recorder.shouldUpdate(configure.leastLineClears, current)) {
                recorder.update(solution, current);
            }
        }

    private:
        Mover<M, C> mover;
        Recorder<C, R> recorder;
    };

    template<class M>
    class Mover<M, TSpinCandidate> {
    public:
        Mover<M, TSpinCandidate>(const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable)
                : factory(factory), moveGenerator(moveGenerator), reachable(reachable) {
        }

        void move(
                const Configure &configure,
                const TSpinCandidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount,
                PerfectClearFinder<M, TSpinCandidate> *finder
        ) {
            assert(0 < candidate.leftLine);

            auto nextLeftNumOfT = pieceType == core::PieceType::T ? candidate.leftNumOfT - 1 : candidate.leftNumOfT;

            moveGenerator.search(moves, candidate.field, pieceType, candidate.leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(pieceType, move.rotateType);

                auto freeze = core::Field(candidate.field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                auto &operation = solution[candidate.depth];
                operation.pieceType = pieceType;
                operation.rotateType = move.rotateType;
                operation.x = move.x;
                operation.y = move.y;

                int tSpinAttack = getAttackIfTSpin(
                        reachable, factory, candidate.field, pieceType, move, numCleared, candidate.b2b
                );

                int nextSoftdropCount = move.harddrop ? candidate.softdropCount : candidate.softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? candidate.lineClearCount + 1 : candidate.lineClearCount;
                int nextCurrentCombo = 0 < numCleared ? candidate.currentCombo + 1 : 0;
                int nextMaxCombo = candidate.maxCombo < nextCurrentCombo ? nextCurrentCombo : candidate.maxCombo;
                int nextTSpinAttack = candidate.tSpinAttack + tSpinAttack;
                bool nextB2b = 0 < numCleared ? (tSpinAttack != 0 || numCleared == 4) : candidate.b2b;

                auto nextDepth = candidate.depth + 1;

                int nextLeftLine = candidate.leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto bestCandidate = TSpinCandidate{
                            freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                            nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                            nextTSpinAttack, nextB2b, nextLeftNumOfT,
                    };
                    finder->accept(configure, solution, bestCandidate);
                    return;
                }

                if (configure.maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                auto nextCandidate = TSpinCandidate{
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

    template<>
    class Recorder<TSpinCandidate, TSpinRecord> {
    public:
        [[nodiscard]] const TSpinRecord& best() const {
            return best_;
        }

        void clear() {
            best_ = TSpinRecord{
                    std::vector<Operation>{},
                    INT_MAX,
                    INT_MAX,
                    INT_MAX,
                    0,
            };
        }

        void update(const Solution &solution, const TSpinCandidate &current) {
            assert(!best_.solution.empty());
            best_ = TSpinRecord{
                    solution, current.softdropCount, current.holdCount, current.lineClearCount,
                    current.maxCombo, current.tSpinAttack
            };
        }

        [[nodiscard]] bool isWorseThanBest(bool leastLineClears, const TSpinCandidate &current) const {
            if (current.leftNumOfT == 0) {
                if (current.tSpinAttack != best_.tSpinAttack) {
                    return current.tSpinAttack < best_.tSpinAttack;
                }

                return best_.softdropCount < current.softdropCount;
            }

            return false;
        }

        [[nodiscard]] bool shouldUpdate(bool leastLineClears, const TSpinCandidate &newRecord) const {
            if (best_.solution.empty()) {
                return true;
            }

            if (leastLineClears) {
                return shouldUpdateLeastLineClear(best_, newRecord);
            } else {
                return shouldUpdateMostLineClear(best_, newRecord);
            }
        }

    private:
        TSpinRecord best_;

        [[nodiscard]] bool shouldUpdateLeastLineClear(
                const TSpinRecord &oldRecord, const TSpinCandidate &newRecord
        ) const {
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

        [[nodiscard]] bool shouldUpdateMostLineClear(
                const TSpinRecord &oldRecord, const TSpinCandidate &newRecord
        ) const {
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
    };

    template<class M = core::srs::MoveGenerator>
    class PerfectFinder {
    public:
        PerfectFinder<M>(const core::Factory &factory, M &moveGenerator)
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

            // Initialize configure
            const Configure configure{
                    pieces,
                    movePool,
                    maxDepth,
                    static_cast<int>(pieces.size()),
                    leastLineClears,
            };

            switch (searchTypes) {
                case SearchTypes::TSpin: {
                    // Count up T
                    int leftNumOfT = std::count(pieces.begin(), pieces.end(), core::PieceType::T);

                    // Create candidate
                    TSpinCandidate candidate = holdEmpty
                                               ? TSpinCandidate{freeze, 0, -1, maxLine, 0, 0, 0, 0,
                                                                initCombo, initCombo, 0, true, leftNumOfT}
                                               : TSpinCandidate{freeze, 1, 0, maxLine, 0, 0, 0, 0,
                                                                initCombo, initCombo, 0, true, leftNumOfT};

                    auto finder = PerfectClearFinder<M, TSpinCandidate, TSpinRecord>(factory, moveGenerator, reachable);
                    return finder.run(configure, candidate);
                }
            }

            assert(false);
            throw std::runtime_error("Illegal search types: value=" + std::to_string(searchTypes));
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        ) {
            return run(field, pieces, maxDepth, maxLine, holdEmpty, true, 0, SearchTypes::TSpin);
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };
}

#endif //FINDER_PERFECT_HPP
