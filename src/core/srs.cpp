#include <cassert>

#include "srs.hpp"

namespace core::srs {
    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        assert((fromRotate + 1) % 4 == toRotate);
        auto &toBlocks = piece.blocks[toRotate];
        return right(field, piece, fromRotate, toBlocks, fromX, fromY);
    }

    int right(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    ) {
        int fromLeftX = fromX + toBlocks.minX;
        int fromLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * Piece::MaxOffsetRotate90;
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
        assert((fromRotate + 3) % 4 == toRotate);
        auto &toBlocks = piece.blocks[toRotate];
        return left(field, piece, fromRotate, toBlocks, fromX, fromY);
    }

    int left(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    ) {
        int fromLeftX = fromX + toBlocks.minX;
        int fromLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * Piece::MaxOffsetRotate90;
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

    int rotate180(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    ) {
        assert((fromRotate + 2) % 4 == toRotate);
        auto &toBlocks = piece.blocks[toRotate];
        return rotate180(field, piece, fromRotate, toBlocks, fromX, fromY);
    }

    int rotate180(
            const Field &field, const Piece &piece, RotateType fromRotate, const Blocks &toBlocks, int fromX, int fromY
    ) {
        int fromLeftX = fromX + toBlocks.minX;
        int fromLowerY = fromY + toBlocks.minY;

        auto head = fromRotate * Piece::MaxOffsetRotate180;
        int width = FIELD_WIDTH - toBlocks.width;
        for (int index = head; index < head + piece.rotate180OffsetsSize; ++index) {
            auto &offset = piece.rotate180Offsets[index];
            int toX = fromLeftX + offset.x;
            int toY = fromLowerY + offset.y;
            if (0 <= toX && toX <= width && 0 <= toY && field.canPutAtMaskIndex(toBlocks, toX, toY)) {
                return index;
            }
        }

        return -1;
    }
}
