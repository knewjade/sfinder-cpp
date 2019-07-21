#include "spins.hpp"

namespace sfinder {
    namespace {
        bool isBlock(const core::Field &field, int x, int y) {
            if (x < 0 || core::kFieldWidth <= x || y < 0) {
                return true;
            }
            return !field.isEmpty(x, y);
        }
    }

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType) {
        assert(0 <= x && x < core::kFieldWidth);
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

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, core::RotateType rotateType, int toX, int toY, int numCleared, bool b2b
    ) {
        if (pieceType != core::PieceType::T) {
            return 0;
        }

        if (numCleared == 0) {
            return 0;
        }

        auto shapes = getTSpinShape(field, toX, toY, rotateType);
        if (shapes == TSpinShapes::NoShape) {
            return 0;
        }

        if (!reachable.checks(field, pieceType, rotateType, toX, toY, core::kMaxFieldHeight)) {
            return 0;
        }

        if (shapes == TSpinShapes::RegularShape) {
            int baseAttack = numCleared * 2;
            return b2b ? baseAttack + 1 : baseAttack;
        }

        // Checks mini or regular (Last SRS test pattern)

        auto &piece = factory.get(pieceType);
        auto &toBlocks = factory.get(pieceType, rotateType);

        // Rotate right
        {
            // Direction before right rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 3) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = core::kFieldWidth - fromBlocks.width;
            for (int index = head; index < head + core::Piece::kOffsetsSize; ++index) {
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
            int width = core::kFieldWidth - fromBlocks.width;
            for (int index = head; index < head + core::Piece::kOffsetsSize; ++index) {
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

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    ) {
        return getAttackIfTSpin(reachable, factory, field, pieceType, move.rotateType, move.x, move.y, numCleared, b2b);
    }
}
