#include "spins.hpp"

namespace finder {
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

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType) {
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

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
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

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = kFieldWidth - fromBlocks.width;
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
            int width = kFieldWidth - fromBlocks.width;
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
}
