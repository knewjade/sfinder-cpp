#include "srs.hpp"

namespace core::srs {
    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        auto &toBlocks = piece.blocks[toRotate];
        return right(field, piece, fromRotate, toBlocks, fromX, fromY);
    }

    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    ) {
        int fromLeftX = fromX + toBlocks.minX;
        int fromLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * 5;
        int width = FIELD_WIDTH - toBlocks.width;
        for (int index = head; index < head + piece.offsetsSize; ++index) {
            auto &offset = piece.rightOffsets[index];
            int toX = fromLeftX + offset.x;
            int toY = fromLowerY + offset.y;
            if (0 <= toX && toX <= width && 0 <= toY && field.canPutAtMaskIndex(toBlocks, toX, toY)) {
                return index;
            }
        }

        return -1;
    }

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        auto &toBlocks = piece.blocks[toRotate];
        return left(field, piece, fromRotate, toBlocks, fromX, fromY);
    }

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    ) {
        int fromLeftX = fromX + toBlocks.minX;
        int fromLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * 5;
        int width = FIELD_WIDTH - toBlocks.width;
        for (int index = head; index < head + piece.offsetsSize; ++index) {
            auto &offset = piece.leftOffsets[index];
            int toX = fromLeftX + offset.x;
            int toY = fromLowerY + offset.y;
            if (0 <= toX && toX <= width && 0 <= toY && field.canPutAtMaskIndex(toBlocks, toX, toY)) {
                return index;
            }
        }

        return -1;
    }
}
