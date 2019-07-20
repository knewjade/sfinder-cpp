#ifndef FINDER_PERFECT_CLEAR_FINDER_HPP
#define FINDER_PERFECT_CLEAR_FINDER_HPP

#include "../types.hpp"
#include "../spins.hpp"
#include "../../core/factory.hpp"
#include "../../core/moves.hpp"

namespace finder::perfect_clear {
    struct Candidate {
        const core::Field field;
        const int currentIndex;
        const int holdIndex;
        const int leftLine;
        const int depth;
        const int softdropCount;
        const int holdCount;
        const int lineClearCount;
        const int currentCombo;
        const int maxCombo;
        const int tSpinAttack;
        const bool b2b;
        const int leftNumOfT;
    };

    struct Configure {
        const std::vector<core::PieceType> &pieces;
        std::vector<std::vector<core::Move>> &movePool;
        const int maxDepth;
        const int pieceSize;
    };

    struct Record {
        Solution solution;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int maxCombo;
        int tSpinAttack;
    };

    namespace comparators {
        struct LeastSoftdrop_LeastLineClear_LeastHold {
            bool shouldUpdate(
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

            bool isWorseThanBest(const Record &best, const Candidate &current) {
                if (current.leftNumOfT == 0) {
                    if (current.tSpinAttack != best.tSpinAttack) {
                        return current.tSpinAttack < best.tSpinAttack;
                    }

                    return best.softdropCount < current.softdropCount;
                }

                return false;
            }
        };

        struct LeastSoftdrop_MostCombo_MostLineClear_LeastHold {
            bool shouldUpdate(
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

            bool isWorseThanBest(const Record &best, const Candidate &current) {
                if (current.leftNumOfT == 0) {
                    if (current.tSpinAttack != best.tSpinAttack) {
                        return current.tSpinAttack < best.tSpinAttack;
                    }

                    return best.softdropCount < current.softdropCount;
                }

                return false;
            }
        };

        struct Faster {
            bool shouldUpdate(const Record &oldRecord, const Record &newRecord) {
                return true;
            }

            bool isWorseThanBest(const Record &best, const Candidate &current) {
                return best.solution[0].x != -1;
            }
        };
    }

    namespace {
        inline bool validate(const core::Field &field, int maxLine) {
            int sum = maxLine - field.getBlockOnX(0, maxLine);
            for (int x = 1; x < core::kFieldWidth; x++) {
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

    template<class T = core::srs::MoveGenerator, class C = comparators::Faster>
    class Finder {
    public:
        Finder<T, C>(const core::Factory &factory, T &moveGenerator) :
                factory_(factory), moveGenerator_(moveGenerator),
                reachable_(core::srs_rotate_end::Reachable(factory)), comparator_(C{}) {
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        ) {
            return run(field, pieces, maxDepth, maxLine, holdEmpty, 0);
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty, int initCombo
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
            };

            // Count up T
            int leftNumOfT = static_cast<int>(std::count(pieces.begin(), pieces.end(), core::PieceType::T));

            // Create candidate
            Candidate candidate = holdEmpty ? Candidate{
                    freeze, 0, -1, maxLine, 0, 0, 0, 0, initCombo, initCombo, 0, true, leftNumOfT,
            } : Candidate{
                    freeze, 1, 0, maxLine, 0, 0, 0, 0, initCombo, initCombo, 0, true, leftNumOfT
            };

            // Create current record & best record
            best_ = Record{
                    std::vector(solution),
                    INT_MAX,
                    INT_MAX,
                    INT_MAX,
                    0,
            };

            if (comparator_.isWorseThanBest(best_, candidate)) {
                return kNoSolution;
            }
            // Execute
            search(configure, candidate, solution);

            return best_.solution[0].x == -1 ? kNoSolution : std::vector<Operation>(best_.solution);
        }

    private:
        const core::Factory &factory_;
        T &moveGenerator_;
        core::srs_rotate_end::Reachable reachable_;
        Record best_;
        C comparator_;

        void search(
                const Configure &configure,
                const Candidate &candidate,
                Solution &solution
        ) {
            if (comparator_.isWorseThanBest(best_, candidate)) {
                return;
            }

            auto depth = candidate.depth;

            auto &pieces = configure.pieces;
            auto &moves = configure.movePool[depth];

            auto currentIndex = candidate.currentIndex;
            assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
            auto holdIndex = candidate.holdIndex;
            assert(-1 <= holdIndex && holdIndex < static_cast<long long int>(configure.pieceSize));

            auto holdCount = candidate.holdCount;

            bool canUseCurrent = currentIndex < configure.pieceSize;
            if (canUseCurrent) {
                assert(static_cast<unsigned int>(currentIndex) < pieces.size());
                auto &current = pieces[currentIndex];

                moves.clear();
                move(configure, candidate, solution, moves, current, currentIndex + 1, holdIndex, holdCount);
            }

            if (0 <= holdIndex) {
                assert(static_cast<unsigned int>(holdIndex) < pieces.size());

                // Hold exists
                if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                    auto &hold = pieces[holdIndex];

                    moves.clear();
                    move(configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex, holdCount + 1);
                }
            } else {
                assert(canUseCurrent);

                // Empty hold
                auto nextIndex = currentIndex + 1;
                assert(static_cast<unsigned int>(nextIndex) < pieces.size() + 1);

                if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                    assert(static_cast<unsigned int>(nextIndex) < pieces.size());
                    auto &next = pieces[nextIndex];

                    moves.clear();
                    move(configure, candidate, solution, moves, next, nextIndex + 1, currentIndex, holdCount + 1);
                }
            }
        }

        void move(
                const Configure &configure,
                const Candidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount
        ) {
            auto depth = candidate.depth;
            auto maxDepth = configure.maxDepth;
            auto &field = candidate.field;

            auto leftLine = candidate.leftLine;
            assert(0 < leftLine);

            auto softdropCount = candidate.softdropCount;
            auto lineClearCount = candidate.lineClearCount;

            auto currentCombo = candidate.currentCombo;
            auto maxCombo = candidate.maxCombo;

            auto currentTSpinAttack = candidate.tSpinAttack;
            auto currentB2b = candidate.b2b;

            auto nextLeftNumOfT = pieceType == core::PieceType::T ? candidate.leftNumOfT - 1 : candidate.leftNumOfT;

            moveGenerator_.search(moves, field, pieceType, leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory_.get(pieceType, move.rotateType);

                auto freeze = core::Field(field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                solution[depth].pieceType = pieceType;
                solution[depth].rotateType = move.rotateType;
                solution[depth].x = move.x;
                solution[depth].y = move.y;

                int tSpinAttack = getAttackIfTSpin(reachable_, factory_, field, pieceType, move, numCleared,
                                                   currentB2b);

                int nextSoftdropCount = move.harddrop ? softdropCount : softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? lineClearCount + 1 : lineClearCount;
                int nextCurrentCombo = 0 < numCleared ? currentCombo + 1 : 0;
                int nextMaxCombo = maxCombo < nextCurrentCombo ? nextCurrentCombo : maxCombo;
                int nextTSpinAttack = currentTSpinAttack + tSpinAttack;
                bool nextB2b = 0 < numCleared ? (tSpinAttack != 0 || numCleared == 4) : currentB2b;

                int nextLeftLine = leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto record = Record{
                            solution, nextSoftdropCount, nextHoldCount, nextLineClearCount, nextMaxCombo,
                            nextTSpinAttack
                    };
                    accept(configure, record);
                    return;
                }

                auto nextDepth = depth + 1;
                if (maxDepth <= nextDepth) {
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
                search(configure, nextCandidate, solution);
            }
        }

        void accept(const Configure &configure, const Record &record) {
            assert(!best_.solution.empty());

            if (best_.solution[0].x == -1 || comparator_.shouldUpdate(best_, record)) {
                best_ = Record(record);
            }
        }
    };
}

#endif //FINDER_PERFECT_CLEAR_FINDER_HPP
