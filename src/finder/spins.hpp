#ifndef FINDER_SPINS_HPP
#define FINDER_SPINS_HPP

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

namespace finder {
    enum TSpinShapes {
        NoShape,
        RegularShape,
        MiniOrTSTShape,
    };

    namespace {
        constexpr int kFieldWidth = 10;
        constexpr int kFieldHeight = 24;

        bool isBlock(const core::Field &field, int x, int y) {
            if (x < 0 || kFieldWidth <= x || y < 0) {
                return true;
            }
            return !field.isEmpty(x, y);
        }
    }

    inline TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y);

        auto b1 = isBlock(field, x - 1, y - 1);
        auto b2 = isBlock(field, x - 1, y + 1);
        auto b3 = isBlock(field, x + 1, y - 1);
        auto b4 = isBlock(field, x + 1, y + 1);

        auto shape = (b1 || b2) && (b1 || b3) && (b1 || b4) && (b2 || b3) && (b2 || b4) && (b3 || b4);
        if (!shape) {
            return TSpinShapes::NoShape;
        }

        switch (rotateType) {
            case core::RotateType::Spawn:
                return b1 && b3 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Right:
                return b1 && b2 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Reverse:
                return b2 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Left:
                return b3 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
        }

        assert(false);
        return TSpinShapes::NoShape;
    }

    //  Caution: mini attack is 0
    template<bool Allow180, bool AllowSoftdropTap>
    constexpr int getAttackIfTSpin(
            core::srs::MoveGenerator<Allow180, AllowSoftdropTap> &moveGenerator,
            core::srs_rotate_end::Reachable<Allow180, AllowSoftdropTap> &reachable,
            const core::Factory &factory, const core::Field &field,
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

        if (!reachable.checks(field, pieceType, rotateType, move.x, move.y, kFieldHeight)) {
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

            // Last SRS offset
            int lastOffsetIndex = fromRotate * 5 + (piece.offsetsSize - 1);
            auto &offset = piece.rightOffsets[lastOffsetIndex];

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            int fromLeftX = toLeftX - offset.x;
            int fromLowerY = toLowerY - offset.y;

            int width = kFieldWidth - fromBlocks.width;

            if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                int fromX = toX - offset.x;
                int fromY = toY - offset.y;
                int srsResult = core::srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                if (srsResult == lastOffsetIndex) {
                    // T-Spin Regular if come back to the place
                    if (moveGenerator.canReach(field, pieceType, fromRotate, fromX, fromY, kFieldHeight)) {
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }
                }

                // Mini
            }
        }

        // Rotate left
        {
            // Direction before left rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 1) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Last SRS offset
            int lastOffsetIndex = fromRotate * 5 + (piece.offsetsSize - 1);
            auto &offset = piece.leftOffsets[lastOffsetIndex];

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            int fromLeftX = toLeftX - offset.x;
            int fromLowerY = toLowerY - offset.y;

            int width = kFieldWidth - fromBlocks.width;

            if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                int fromX = toX - offset.x;
                int fromY = toY - offset.y;
                int srsResult = core::srs::left(field, piece, fromRotate, toBlocks, fromX, fromY);
                if (srsResult == lastOffsetIndex) {
                    // T-Spin Regular if come back to the place
                    if (moveGenerator.canReach(field, pieceType, fromRotate, fromX, fromY, kFieldHeight)) {
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }
                }

                // Mini
            }
        }

        return b2b ? 1 : 0;
    }

    template<bool AlwaysRegularAttack, bool Allow180, bool AllowSoftdropTap>
    constexpr int getAttackIfAllSpins(
            core::srs::MoveGenerator<Allow180, AllowSoftdropTap> &moveGenerator,
            core::srs_rotate_end::Reachable<Allow180, AllowSoftdropTap> &reachable,
            const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    ) {
        if (pieceType == core::PieceType::O) {
            return 0;
        }

        if (numCleared == 0) {
            return 0;
        }

        auto rotateType = move.rotateType;
        if (!reachable.checks(field, pieceType, rotateType, move.x, move.y, kFieldHeight)) {
            return 0;
        }

        auto &blocks = factory.get(pieceType, move.rotateType);

        auto x = move.x;
        auto y = move.y;
        if (!(
                (x + blocks.minX - 1 < 0 || !field.canPut(blocks, x - 1, y))
                && (kFieldWidth <= x + blocks.maxX + 1 || !field.canPut(blocks, x + 1, y))
                && !field.canPut(blocks, x, y + 1)
        )) {
            // It's not immobile
            return 0;
        }

        // It's spin
        int baseAttack = numCleared * 2;

        if constexpr (!AlwaysRegularAttack) {
            auto &piece = factory.get(pieceType);

            // Rotate right
            {
                // Direction before right rotation
                auto fromRotate = static_cast<core::RotateType>((rotateType + 3) % 4);
                auto &fromBlocks = factory.get(pieceType, fromRotate);

                // Last SRS offset
                int lastOffsetIndex = fromRotate * 5;
                auto &offset = piece.rightOffsets[lastOffsetIndex];

                if (offset.x == 0 && offset.y == 0) {
                    if (0 <= x + fromBlocks.minX &&
                        x + fromBlocks.maxX < kFieldWidth &&
                        0 <= y + fromBlocks.minY &&
                        field.canPut(fromBlocks, x, y) &&
                        moveGenerator.canReach(field, pieceType, fromRotate, x, y, kFieldHeight)) {
                        // NOT kicked
                        // Regular is enable or regular spin
                        return b2b ? baseAttack + 1 : baseAttack;
                    }
                }
            }

            // Rotate left
            {
                // Direction before left rotation
                auto fromRotate = static_cast<core::RotateType>((rotateType + 1) % 4);
                auto &fromBlocks = factory.get(pieceType, fromRotate);

                // Last SRS offset
                int lastOffsetIndex = fromRotate * 5;
                auto &offset = piece.leftOffsets[lastOffsetIndex];

                if (offset.x == 0 && offset.y == 0) {
                    if (0 <= x + fromBlocks.minX &&
                        x + fromBlocks.maxX < kFieldWidth &&
                        0 <= y + fromBlocks.minY &&
                        field.canPut(fromBlocks, x, y) &&
                        moveGenerator.canReach(field, pieceType, fromRotate, x, y, kFieldHeight)) {
                        // NOT kicked
                        // Regular is enable or regular spin
                        return b2b ? baseAttack + 1 : baseAttack;
                    }
                }
            }

            // Need kick

            // If `AlwaysRegularAttack` is false, mini attack is 0.
            // Judged as mini if doesn't clear every line it occupies.
            if (numCleared != blocks.height) {
                // mini
                return b2b ? 1 : 0;
            }

            // NOT mini
        }

        // If `AlwaysRegularAttack` is true, all spins attack is judged as regular
        // Regular is enable or regular spin
        return b2b ? baseAttack + 1 : baseAttack;
    }
}

#endif //FINDER_SPINS_HPP
