#include "field.hpp"

namespace core {
    namespace {
        const int FIELD_WIDTH = 10;
        const int MAX_FIELD_HEIGHT = 24;
        const uint64_t VALID_BOARD_RANGE = 0xfffffffffffffffL;

        uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            return static_cast<uint64_t >(1) << (x + y * FIELD_WIDTH);
        }
    }

    void Field::setBlock(int x, int y) {
        assert(0 <= x && x < FIELD_WIDTH);
        assert(0 <= y && y < MAX_FIELD_HEIGHT);

        int index = y / 6;
        boards[index] |= getXMask(x, y - 6 * index);
    }

    void Field::removeBlock(int x, int y) {
        assert(0 <= x && x < FIELD_WIDTH);
        assert(0 <= y && y < MAX_FIELD_HEIGHT);

        int index = y / 6;
        boards[index] &= ~getXMask(x, y - 6 * index);
    }

    bool Field::isEmpty(int x, int y) const {
        assert(0 <= x && x < FIELD_WIDTH);
        assert(0 <= y && y < MAX_FIELD_HEIGHT);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);
        return (boards[index] & mask) == 0;
    }

    void Field::put(const Blocks &blocks, int x, int y) {
        putAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    void Field::putAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) {
        assert(0 <= leftX && leftX < FIELD_WIDTH);
        assert(0 <= lowerY && lowerY < MAX_FIELD_HEIGHT);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        boards[index] |= mask.low;
        if (index <= 2) {
            boards[index + 1] |= mask.high;
        }
    }

    bool Field::canPut(const Blocks &blocks, int x, int y) const {
        return canPutAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    bool Field::canPutAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) const {
        assert(0 <= leftX && leftX < FIELD_WIDTH);
        assert(0 <= lowerY && lowerY < MAX_FIELD_HEIGHT);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        if (index <= 2) {
            return (boards[index] & mask.low) == 0 && (boards[index + 1] && mask.high) == 0;
        }

        return (boards[index] & mask.low) == 0;
    }

    bool Field::isOnGround(const Blocks &blocks, int x, int y) {
        return y <= -blocks.minY || !canPut(blocks, x, y - 1);
    }

    LineKey getDeleteKey(Bitboard board) {
        uint64_t a1010101010 = 768614336404564650L;
        auto b1 = (board & a1010101010) >> 1 & board;
        uint64_t a0101010000 = 378672165735973200L;
        auto b2 = (b1 & a0101010000) >> 4 & b1;
        uint64_t a0000010100 = 22540009865236500L;
        auto b3 = (b2 & a0000010100) >> 2 & b2;
        uint64_t a0000000100 = 4508001973047300L;
        return (b3 & a0000000100) >> 2 & b3;
    }

    void
    Field::deleteLine_(LineKey deleteKeyLow, LineKey deleteKeyMidLow, LineKey deleteKeyMidHigh, LineKey deleteKeyHigh) {
        // 下半分
        long newXBoardLow = deleteLine(xBoardLow, deleteKeyLow);

        long newXBoardMidLow = deleteLine(xBoardMidLow, deleteKeyMidLow);

        int deleteLineLow = bitCount(deleteKeyLow);

        long low = (newXBoardLow | (newXBoardMidLow << (6 - deleteLineLow) * 10)) & VALID_BOARD_RANGE;
        long midLow = newXBoardMidLow >> deleteLineLow * 10;

        int deleteLineMidLow = bitCount(deleteKeyMidLow);
        int deleteLineBottom = deleteLineLow + deleteLineMidLow;

        // 上半分
        long newXBoardMidHigh = deleteLine(xBoardMidHigh, deleteKeyMidHigh);

        long newXBoardHigh = deleteLine(xBoardHigh, deleteKeyHigh);

        int deleteLineMidHigh = bitCount(deleteKeyMidHigh);

        long midHigh = (newXBoardMidHigh | (newXBoardHigh << (6 - deleteLineMidHigh) * 10)) & VALID_BOARD_RANGE;
        long high = newXBoardHigh >> deleteLineMidHigh * 10;

        // 上半分と下半分をマージ
        if (deleteLineBottom < 6) {
            xBoardLow = low;
            xBoardMidLow = (midLow | (midHigh << (6 - deleteLineBottom) * 10)) & VALID_BOARD_RANGE;
            xBoardMidHigh =
                    ((midHigh >> deleteLineBottom * 10) | (high << (6 - deleteLineBottom) * 10)) & VALID_BOARD_RANGE;
            xBoardHigh = high >> deleteLineBottom * 10;
        } else {
            int slide = deleteLineBottom - 6;
            xBoardLow = (low | (midHigh << (6 - slide) * 10)) & VALID_BOARD_RANGE;
            xBoardMidLow = ((midHigh >> slide * 10) | (high << (6 - slide) * 10)) & VALID_BOARD_RANGE;
            xBoardMidHigh = high >> slide * 10;
            xBoardHigh = 0L;
        }
    }

    void Field::clearLine() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);
    }

    LineKey Field::clearLineReturnKey() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);

        return deleteKeyLow | (deleteKeyMidLow << 1) | (deleteKeyMidHigh << 2) | (deleteKeyHigh << 3);
    }

    std::string Field::toString(int height) {
        using namespace std::literals::string_literals;

        auto str = ""s;
        for (int y = height - 1; 0 <= y; --y) {
            for (int x = 0; x < 10; ++x) {
                if (isEmpty(x, y)) {
                    str += "_";
                } else {
                    str += "X";
                }
            }
            str += "\n";
        }
        return str;
    }

    int rotateRight(
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

    int rotateLeft(
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
