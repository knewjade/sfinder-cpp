#ifndef CORE_PERFECT_HPP
#define CORE_PERFECT_HPP

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    enum PriorityTypes {
        LeastSoftdrop_LeastLineClear_LeastHold,
        LeastSoftdrop_MostCombo_MostLineClear_LeastHold,
    };

    enum TSpinShapes {
        NoShape,
        RegularShape,
        MiniOrTSTShape,
    };

    struct Candidate {
        const core::Field &field;
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
        const bool leastLineClears;
    };

    struct Operation {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
    };

    using Solution = std::vector<Operation>;
    inline const Solution kNoSolution = std::vector<Operation>();

    struct Record {
        Solution solution;
        int softdropCount;
        int holdCount;
        int lineClearCount;
        int maxCombo;
        int tSpinAttack;
    };

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType);

    constexpr int FIELD_HEIGHT = 24;
    constexpr int FIELD_WIDTH = 10;

    // TODO Allow180
    template<bool Allow180>
    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable<Allow180> &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    ) {
        if (pieceType != core::PieceType::T) {
            return 0;
        }

        if (numCleared == 0) {
            return 0;
        }

        auto rotateType = move.rotateType;
        auto shapes = getTSpinShape(field, move.x, move.y, rotateType);
        if (shapes == TSpinShapes::NoShape) {
            return 0;
        }

        if (!reachable.checks(field, pieceType, rotateType, move.x, move.y, FIELD_HEIGHT)) {
            return 0;
        }

        if (shapes == TSpinShapes::RegularShape) {
            int baseAttack = numCleared * 2;
            return b2b ? baseAttack + 1 : baseAttack;
        }

        // Checks mini or regular (Last SRS test pattern)

        auto &piece = factory.get(pieceType);
        auto &toBlocks = factory.get(pieceType, rotateType);

        auto toX = move.x;
        auto toY = move.y;

        // Rotate right
        {
            // Direction before right rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 3) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.rightOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                    field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int srsResult = core::srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (0 <= srsResult && srsResult % 5 == 4) {
                        // T-Spin Regular
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }

                    // Mini or No T-Spin
                }
            }
        }

        // Rotate left
        {
            // Direction before left rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 1) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.leftOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                    field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int srsResult = core::srs::left(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (0 <= srsResult && srsResult % 5 == 4) {
                        // T-Spin Regular
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }

                    // Mini or No T-Spin
                }
            }
        }

        return 0;
    }

    template<bool Allow180 = false, class T = core::srs::MoveGenerator<Allow180>>
    class PerfectFinder {
    public:
        PerfectFinder(const core::Factory &factory, T &moveGenerator)
                : factory(factory), moveGenerator(moveGenerator), reachable(core::srs_rotate_end::Reachable(factory)) {
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        );

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty, bool leastLineClears, int initCombo
        );

    private:
        const core::Factory &factory;
        T &moveGenerator;
        core::srs_rotate_end::Reachable<Allow180> reachable;
        Record best;

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

#endif //CORE_PERFECT_HPP
