#include "srs.hpp"

namespace core::srs {
    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        auto &toBlocks = piece.blocks[toRotate];

        int toLeftX = fromX + toBlocks.minX;
        int toLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * 5;
        int width = FIELD_WIDTH - toBlocks.width;
        for (int index = head; index < head + piece.offsetsSize; ++index) {
            auto &offset = piece.rightOffsets[index];
            int x = toLeftX + offset.x;
            int y = toLowerY + offset.y;
            if (0 <= x && x <= width && 0 <= y && field.canPutAtMaskIndex(toBlocks, x, y)) {
                return index;
            }
        }

        return -1;
    }

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        auto &toBlocks = piece.blocks[toRotate];

        int toLeftX = fromX + toBlocks.minX;
        int toLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * 5;
        int width = FIELD_WIDTH - toBlocks.width;
        for (int index = head; index < head + piece.offsetsSize; ++index) {
            auto &offset = piece.leftOffsets[index];
            int x = toLeftX + offset.x;
            int y = toLowerY + offset.y;
            if (0 <= x && x <= width && 0 <= y && field.canPutAtMaskIndex(toBlocks, x, y)) {
                return index;
            }
        }

        return -1;
    }
}
