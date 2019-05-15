#include "field.hpp"

namespace core {
    namespace {
        const uint64_t VALID_BOARD_RANGE = 0xfffffffffffffffL;

        uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            return 1LLU << (x + y * FIELD_WIDTH);
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

    void Field::remove(const Blocks &blocks, int x, int y) {
        putAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    void Field::removeAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) {
        assert(0 <= leftX && leftX < FIELD_WIDTH);
        assert(0 <= lowerY && lowerY < MAX_FIELD_HEIGHT);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        boards[index] &= ~mask.low;
        if (index <= 2) {
            boards[index + 1] &= ~mask.high;
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
            return (boards[index] & mask.low) == 0 && (boards[index + 1] & mask.high) == 0;
        }

        return (boards[index] & mask.low) == 0;
    }

    bool Field::isOnGround(const Blocks &blocks, int x, int y) const {
        return y <= -blocks.minY || !canPut(blocks, x, y - 1);
    }

    int Field::getYOnHarddrop(const Blocks &blocks, int x, int startY) const {
        int min = -blocks.minY;
        for (int y = startY - 1; min <= y; y--)
            if (!canPut(blocks, x, y))
                return y + 1;
        return min;
    }

    bool Field::canReachOnHarddrop(const Blocks &blocks, int x, int y) const {
        const int leftX = x + blocks.minX;
        const int lowerY = y + blocks.minY;

        assert(0 <= leftX && leftX < FIELD_WIDTH);
        assert(0 <= lowerY && lowerY < MAX_FIELD_HEIGHT);

        Collider collider = blocks.harddrop(leftX, lowerY);

        return (boards[0] & collider.boards[0]) == 0 &&
               (boards[1] & collider.boards[1]) == 0 &&
               (boards[2] & collider.boards[2]) == 0 &&
               (boards[3] & collider.boards[3]) == 0;
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

    void Field::deleteLine_(
            LineKey deleteKeyLow, LineKey deleteKeyMidLow, LineKey deleteKeyMidHigh, LineKey deleteKeyHigh
    ) {
        // Lower half
        Bitboard newXBoardLow = deleteLine(xBoardLow, deleteKeyLow);

        Bitboard newXBoardMidLow = deleteLine(xBoardMidLow, deleteKeyMidLow);

        int deleteLineLow = bitCount(deleteKeyLow);

        Bitboard low = (newXBoardLow | (newXBoardMidLow << (6 - deleteLineLow) * 10)) & VALID_BOARD_RANGE;
        Bitboard midLow = newXBoardMidLow >> deleteLineLow * 10;

        int deleteLineMidLow = bitCount(deleteKeyMidLow);
        int deleteLineBottom = deleteLineLow + deleteLineMidLow;

        // Upper half
        Bitboard newXBoardMidHigh = deleteLine(xBoardMidHigh, deleteKeyMidHigh);

        Bitboard newXBoardHigh = deleteLine(xBoardHigh, deleteKeyHigh);

        int deleteLineMidHigh = bitCount(deleteKeyMidHigh);

        Bitboard midHigh = (newXBoardMidHigh | (newXBoardHigh << (6 - deleteLineMidHigh) * 10)) & VALID_BOARD_RANGE;
        Bitboard high = newXBoardHigh >> deleteLineMidHigh * 10;

        // Merge the upper and lower halves
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

    int Field::getBlockOnX(int x, int maxY) const {
        assert(0 <= maxY && maxY <= MAX_FIELD_HEIGHT);

        if (maxY < 12) {
            if (maxY < 6) {
                Bitboard mask = getColumnOneLineBelowY(maxY) << x;
                return bitCount(xBoardLow & mask);
            } else {
                Bitboard fullMask = getColumnOneLineBelowY(6) << x;
                Bitboard mask = getColumnOneLineBelowY(maxY - 6) << x;
                return bitCount(xBoardLow & fullMask)
                       + bitCount(xBoardMidLow & mask);
            }
        } else {
            if (maxY < 18) {
                Bitboard fullMask = getColumnOneLineBelowY(6) << x;
                Bitboard mask = getColumnOneLineBelowY(maxY - 12) << x;
                return bitCount(xBoardLow & fullMask)
                       + bitCount(xBoardMidLow & fullMask) +
                       bitCount(xBoardMidHigh & mask);
            } else {
                Bitboard fullMask = getColumnOneLineBelowY(6) << x;
                Bitboard mask = getColumnOneLineBelowY(maxY - 18) << x;
                return bitCount(xBoardLow & fullMask)
                       + bitCount(xBoardMidLow & fullMask)
                       + bitCount(xBoardMidHigh & fullMask)
                       + bitCount(xBoardHigh & mask);
            }
        }
    }

    bool Field::isWallBetween(int x, int maxY) const {
        assert(0 <= maxY && maxY <= MAX_FIELD_HEIGHT);

        if (maxY == 0) {
            return true;
        }

        if (maxY < 12) {
            if (maxY < 6) {
                // Check Low
                return isWallBetweenLeft(x, maxY, xBoardLow);
            } else {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow))
                    return false;

                // Check MidLow
                return isWallBetweenLeft(x, maxY - 6, xBoardMidLow);
            }
        } else {
            if (maxY < 18) {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow))
                    return false;

                // Check MidLow
                if (!isWallBetweenLeft(x, 6, xBoardMidLow))
                    return false;

                // Check MidHigh
                return isWallBetweenLeft(x, maxY - 12, xBoardMidHigh);
            } else {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow))
                    return false;

                // Check MidLow
                if (!isWallBetweenLeft(x, 6, xBoardMidLow))
                    return false;

                // Check MidHigh
                if (!isWallBetweenLeft(x, 6, xBoardMidHigh))
                    return false;

                // Check High
                return isWallBetweenLeft(x, maxY - 18, xBoardHigh);
            }
        }
    }

    int Field::clearLineReturnNum() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);

        return bitCount(deleteKeyLow | (deleteKeyMidLow << 1) | (deleteKeyMidHigh << 2) | (deleteKeyHigh << 3));
    }

    std::string Field::toString(int height) const {
        auto str = std::string("");
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

    Field createField(std::string marks) {
        assert(marks.length() < 240);
        assert(marks.length() % 10 == 0);

        int maxY = static_cast<int>(marks.length() / 10);
        Field field = Field();
        for (int y = 0; y < maxY; y++) {
            for (int x = 0; x < 10; x++) {
                char mark = marks.at((maxY - y - 1) * 10 + x);
                if (mark != ' ' && mark != '_') {
                    field.setBlock(x, y);
                }
            }
        }

        return field;
    }
}
