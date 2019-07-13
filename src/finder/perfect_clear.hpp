#ifndef CORE_PERFECT_CLEAR_HPP
#define CORE_PERFECT_CLEAR_HPP

#include "./types.hpp"
#include "../core/factory.hpp"
#include "../core/moves.hpp"

namespace finder {
    enum TSpinShapes {
        NoShape,
        RegularShape,
        MiniOrTSTShape,
    };

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType);

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    );

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, core::RotateType rotateType, int x, int y, int numCleared, bool b2b
    );

    namespace perfect_clear {
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

        template<class T = core::srs::MoveGenerator, class C = Faster>
        class Finder {
        public:
            Finder<T>(const core::Factory &factory, T &moveGenerator)
                    : factory_(factory), moveGenerator_(moveGenerator),
                      reachable_(core::srs_rotate_end::Reachable(factory)),
                      comparator_(C{}) {
            }

            Solution run(
                    const core::Field &field, const std::vector<core::PieceType> &pieces,
                    int maxDepth, int maxLine, bool holdEmpty
            );

            Solution run(
                    const core::Field &field, const std::vector<core::PieceType> &pieces,
                    int maxDepth, int maxLine, bool holdEmpty, int initCombo
            );

        private:
            const core::Factory &factory_;
            T &moveGenerator_;
            core::srs_rotate_end::Reachable reachable_;
            Record best_;
            C comparator_;

            void search(const Configure &configure, const Candidate &candidate, Solution &solution);

            void move(
                    const Configure &configure,
                    const Candidate &candidate,
                    Solution &solution,
                    std::vector<core::Move> &moves,
                    core::PieceType pieceType,
                    int nextIndex,
                    int nextHoldIndex,
                    int nextHoldCount
            );

            void accept(const Configure &configure, const Record &record);
        };
    }
}

#endif //CORE_PERFECT_CLEAR_HPP
