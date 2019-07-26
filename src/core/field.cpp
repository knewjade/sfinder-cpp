#include <cassert>

#include "field.hpp"

namespace core {
    namespace {
        uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < kFieldWidth);
            assert(0 <= y && y < 6);

            return 1LLU << (x + y * kFieldWidthUnsigned);
        }

        uint64_t getLineMask(int y) {
            assert(0 <= y && y < 6);

            return 0x3ffLLU << (y * kFieldWidthUnsigned);
        }

        LineKey extractDeleteKeyLow(LineKey deleteKey) {
            return deleteKey & 0x4010040100401ULL;
        }

        LineKey extractDeleteKeyMidLow(LineKey deleteKey) {
            return (deleteKey & 0x8020080200802ULL) >> 1U;
        }

        LineKey extractDeleteKeyMidHigh(LineKey deleteKey) {
            return (deleteKey & 0x10040100401004ULL) >> 2U;
        }

        LineKey extractDeleteKeyHigh(LineKey deleteKey) {
            return (deleteKey & 0x20080200802008ULL) >> 3U;
        }
    }

    void Field::setBlock(int x, int y) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        boards_[index] |= getXMask(x, y - 6 * index);
    }

    void Field::removeBlock(int x, int y) {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        boards_[index] &= ~getXMask(x, y - 6 * index);
    }

    bool Field::isEmpty(int x, int y) const {
        assert(0 <= x && x < kFieldWidth);
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getXMask(x, y - 6 * index);
        return (boards_[index] & mask) == 0;
    }

    bool Field::isEmptyOnY(int y) const {
        assert(0 <= y && y < kMaxFieldHeight);

        int index = y / 6;
        uint64_t mask = getLineMask(y - 6 * index);
        return (boards_[index] & mask) == 0;
    }

    void Field::put(const Blocks &blocks, int x, int y) {
        putAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    void Field::putAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) {
        assert(0 <= leftX && leftX < kFieldWidth);
        assert(0 <= lowerY && lowerY < kMaxFieldHeight);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        boards_[index] |= mask.low;
        if (index <= 2) {
            boards_[index + 1] |= mask.high;
        }
    }

    void Field::remove(const Blocks &blocks, int x, int y) {
        removeAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    void Field::removeAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) {
        assert(0 <= leftX && leftX < kFieldWidth);
        assert(0 <= lowerY && lowerY < kMaxFieldHeight);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        boards_[index] &= ~mask.low;
        if (index <= 2) {
            boards_[index + 1] &= ~mask.high;
        }
    }

    bool Field::canPut(const Blocks &blocks, int x, int y) const {
        return canPutAtMaskIndex(blocks, x + blocks.minX, y + blocks.minY);
    }

    bool Field::canPutAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) const {
        assert(0 <= leftX && leftX < kFieldWidth);
        assert(0 <= lowerY && lowerY < kMaxFieldHeight);

        int index = lowerY / 6;
        BlocksMask mask = blocks.mask(leftX, lowerY - 6 * index);

        if (index <= 2) {
            return (boards_[index] & mask.low) == 0 && (boards_[index + 1] & mask.high) == 0;
        }

        return (boards_[index] & mask.low) == 0;
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

        assert(0 <= leftX && leftX < kFieldWidth);
        assert(0 <= lowerY && lowerY < kMaxFieldHeight);

        Collider collider = blocks.harddrop(leftX, lowerY);

        return (boards_[0] & collider.boards[0]) == 0 &&
               (boards_[1] & collider.boards[1]) == 0 &&
               (boards_[2] & collider.boards[2]) == 0 &&
               (boards_[3] & collider.boards[3]) == 0;
    }

    LineKey getDeleteKey(Bitboard board) {
        uint64_t a1010101010 = 768614336404564650LLU;
        auto b1 = (board & a1010101010) >> 1U & board;
        uint64_t a0101010000 = 378672165735973200LLU;
        auto b2 = (b1 & a0101010000) >> 4U & b1;
        uint64_t a0000010100 = 22540009865236500LLU;
        auto b3 = (b2 & a0000010100) >> 2U & b2;
        uint64_t a0000000100 = 4508001973047300LLU;
        return (b3 & a0000000100) >> 2U & b3;
    }

    void Field::deleteLine_(
            LineKey deleteKeyLow, LineKey deleteKeyMidLow, LineKey deleteKeyMidHigh, LineKey deleteKeyHigh
    ) {
        // Lower half
        Bitboard newXBoardLow = deleteLine(xBoardLow_, deleteKeyLow);

        Bitboard newXBoardMidLow = deleteLine(xBoardMidLow_, deleteKeyMidLow);

        int deleteLineLow = bitCount(deleteKeyLow);

        Bitboard low = (newXBoardLow | (newXBoardMidLow << (6 - deleteLineLow) * 10U)) & kValidBoardRange;
        Bitboard midLow = newXBoardMidLow >> deleteLineLow * 10U;

        int deleteLineMidLow = bitCount(deleteKeyMidLow);
        int deleteLineBottom = deleteLineLow + deleteLineMidLow;

        // Upper half
        Bitboard newXBoardMidHigh = deleteLine(xBoardMidHigh_, deleteKeyMidHigh);

        Bitboard newXBoardHigh = deleteLine(xBoardHigh_, deleteKeyHigh);

        int deleteLineMidHigh = bitCount(deleteKeyMidHigh);

        Bitboard midHigh = (newXBoardMidHigh | (newXBoardHigh << (6 - deleteLineMidHigh) * 10U)) & kValidBoardRange;
        Bitboard high = newXBoardHigh >> deleteLineMidHigh * 10U;

        // Merge the upper and lower halves
        if (deleteLineBottom < 6) {
            xBoardLow_ = low;
            xBoardMidLow_ = (midLow | (midHigh << (6 - deleteLineBottom) * 10U)) & kValidBoardRange;
            xBoardMidHigh_ =
                    ((midHigh >> deleteLineBottom * 10U) | (high << (6 - deleteLineBottom) * 10U)) & kValidBoardRange;
            xBoardHigh_ = high >> deleteLineBottom * 10U;
        } else {
            int slide = deleteLineBottom - 6;
            xBoardLow_ = (low | (midHigh << (6 - slide) * 10U)) & kValidBoardRange;
            xBoardMidLow_ = ((midHigh >> slide * 10U) | (high << (6 - slide) * 10U)) & kValidBoardRange;
            xBoardMidHigh_ = high >> slide * 10U;
            xBoardHigh_ = 0L;
        }
    }

    void Field::clearLine() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow_);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow_);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh_);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh_);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);
    }

    LineKey Field::clearLineReturnKey() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow_);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow_);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh_);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh_);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);

        return deleteKeyLow | (deleteKeyMidLow << 1U) | (deleteKeyMidHigh << 2U) | (deleteKeyHigh << 3U);
    }

    int Field::getBlockOnX(int x, int maxY) const {
        assert(0 <= maxY && maxY <= kMaxFieldHeight);

        auto ux = static_cast<unsigned int>(x);
        if (maxY < 12) {
            if (maxY < 6) {
                Bitboard mask = getColumnOneLineBelowY(maxY) << ux;
                return bitCount(xBoardLow_ & mask);
            } else {
                Bitboard fullMask = getColumnOneLineBelowY(6) << ux;
                Bitboard mask = getColumnOneLineBelowY(maxY - 6) << ux;
                return bitCount(xBoardLow_ & fullMask)
                       + bitCount(xBoardMidLow_ & mask);
            }
        } else {
            if (maxY < 18) {
                Bitboard fullMask = getColumnOneLineBelowY(6) << ux;
                Bitboard mask = getColumnOneLineBelowY(maxY - 12) << ux;
                return bitCount(xBoardLow_ & fullMask)
                       + bitCount(xBoardMidLow_ & fullMask) +
                       bitCount(xBoardMidHigh_ & mask);
            } else {
                Bitboard fullMask = getColumnOneLineBelowY(6) << ux;
                Bitboard mask = getColumnOneLineBelowY(maxY - 18) << ux;
                return bitCount(xBoardLow_ & fullMask)
                       + bitCount(xBoardMidLow_ & fullMask)
                       + bitCount(xBoardMidHigh_ & fullMask)
                       + bitCount(xBoardHigh_ & mask);
            }
        }
    }

    bool Field::isWallBetween(int x, int maxY) const {
        assert(0 <= maxY && maxY <= kMaxFieldHeight);

        if (maxY == 0) {
            return true;
        }

        if (maxY < 12) {
            if (maxY < 6) {
                // Check Low
                return isWallBetweenLeft(x, maxY, xBoardLow_);
            } else {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow_))
                    return false;

                // Check MidLow
                return isWallBetweenLeft(x, maxY - 6, xBoardMidLow_);
            }
        } else {
            if (maxY < 18) {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow_))
                    return false;

                // Check MidLow
                if (!isWallBetweenLeft(x, 6, xBoardMidLow_))
                    return false;

                // Check MidHigh
                return isWallBetweenLeft(x, maxY - 12, xBoardMidHigh_);
            } else {
                // Check Low
                if (!isWallBetweenLeft(x, 6, xBoardLow_))
                    return false;

                // Check MidLow
                if (!isWallBetweenLeft(x, 6, xBoardMidLow_))
                    return false;

                // Check MidHigh
                if (!isWallBetweenLeft(x, 6, xBoardMidHigh_))
                    return false;

                // Check High
                return isWallBetweenLeft(x, maxY - 18, xBoardHigh_);
            }
        }
    }

    int Field::numOfAllBlocks() const {
        return bitCount(xBoardLow_) + bitCount(xBoardMidLow_) + bitCount(xBoardMidHigh_) + bitCount(xBoardHigh_);
    }

    int Field::clearLineReturnNum() {
        LineKey deleteKeyLow = getDeleteKey(xBoardLow_);
        LineKey deleteKeyMidLow = getDeleteKey(xBoardMidLow_);
        LineKey deleteKeyMidHigh = getDeleteKey(xBoardMidHigh_);
        LineKey deleteKeyHigh = getDeleteKey(xBoardHigh_);

        deleteLine_(deleteKeyLow, deleteKeyMidLow, deleteKeyMidHigh, deleteKeyHigh);

        return bitCount(deleteKeyLow | (deleteKeyMidLow << 1U) | (deleteKeyMidHigh << 2U) | (deleteKeyHigh << 3U));
    }

    void Field::insertBlackLineWithKey(LineKey deleteKey) {
        auto deleteKeyLow = extractDeleteKeyLow(deleteKey);
        int deleteLineLow = bitCount(deleteKeyLow);

        auto deleteKeyMidLow = extractDeleteKeyMidLow(deleteKey);
        int deleteLineMidLow = bitCount(deleteKeyMidLow);

        auto deleteKeyMidHigh = extractDeleteKeyMidHigh(deleteKey);
        int deleteLineMidHigh = bitCount(deleteKeyMidHigh);

        auto deleteKeyHigh = extractDeleteKeyHigh(deleteKey);

        int deleteLine1 = deleteLineLow;
        int deleteLine2 = deleteLineLow + deleteLineMidLow;
        int deleteLine3 = deleteLine2 + deleteLineMidHigh;

        if (deleteLine3 < 6) {
            // Low & MidLow & MidHigh & High
            int leftLine3 = 6 - deleteLine3;
            auto newXBoardHigh = insertBlackLine(
                    (xBoardHigh_ << 10U * deleteLine3) |
                    ((xBoardMidHigh_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            int leftLine2 = 6 - deleteLine2;
            auto newXBoardMidHigh = insertBlackLine(
                    (xBoardMidHigh_ << 10U * deleteLine2) |
                    ((xBoardMidLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
            );

            int leftLine1 = 6 - deleteLine1;
            auto newXBoardMidLow = insertBlackLine(
                    (xBoardMidLow_ << 10U * deleteLine1) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
            );

            auto newXBoardLow = insertBlackLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

            xBoardLow_ = newXBoardLow;
            xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
            xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
            xBoardHigh_ = newXBoardHigh & kValidBoardRange;
        } else if (deleteLine3 < 12) {
            // Low & MidLow & MidHigh
            int deleteLine3_6 = deleteLine3 - 6;
            int leftLine3 = 6 - deleteLine3_6;
            auto newXBoardHigh = insertBlackLine(
                    (xBoardMidHigh_ << 10U * deleteLine3_6) |
                    ((xBoardMidLow_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            if (deleteLine2 < 6) {
                // Low & MidLow & MidHigh
                int leftLine2 = 6 - deleteLine2;
                auto newXBoardMidHigh = insertBlackLine(
                        (xBoardMidHigh_ << 10U * deleteLine2) |
                        ((xBoardMidLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
                );

                int leftLine1 = 6 - deleteLine1;
                auto newXBoardMidLow = insertBlackLine(
                        (xBoardMidLow_ << 10U * deleteLine1) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
                );

                auto newXBoardLow = insertBlackLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

                xBoardLow_ = newXBoardLow;
                xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
                xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
                xBoardHigh_ = newXBoardHigh & kValidBoardRange;
            } else {
                // Low & MidLow
                int deleteLine2_6 = deleteLine2 - 6;
                int leftLine2 = 6 - deleteLine2_6;
                auto newXBoardMidHigh = insertBlackLine(
                        (xBoardMidLow_ << 10U * deleteLine2_6) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
                );

                int leftLine1 = 6 - deleteLine1;
                auto newXBoardMidLow = insertBlackLine(
                        (xBoardMidLow_ << 10U * deleteLine1) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
                );

                auto newXBoardLow = insertBlackLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

                xBoardLow_ = newXBoardLow;
                xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
                xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
                xBoardHigh_ = newXBoardHigh & kValidBoardRange;
            }
        } else {
            // Low & MidLow
            int deleteLine3_12 = deleteLine3 - 12;
            int leftLine3 = 6 - deleteLine3_12;
            auto newXBoardHigh = insertBlackLine(
                    (xBoardMidLow_ << 10U * deleteLine3_12) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            int deleteLine2_6 = deleteLine2 - 6;
            int leftLine2 = 6 - deleteLine2_6;
            auto newXBoardMidHigh = insertBlackLine(
                    (xBoardMidLow_ << 10U * deleteLine2_6) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
            );

            int leftLine1 = 6 - deleteLine1;
            auto newXBoardMidLow = insertBlackLine(
                    (xBoardMidLow_ << 10U * deleteLine1) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
            );

            auto newXBoardLow = insertBlackLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

            xBoardLow_ = newXBoardLow;
            xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
            xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
            xBoardHigh_ = newXBoardHigh & kValidBoardRange;
        }
    }

    void Field::insertWhiteLineWithKey(LineKey deleteKey) {
        auto deleteKeyLow = extractDeleteKeyLow(deleteKey);
        int deleteLineLow = bitCount(deleteKeyLow);

        auto deleteKeyMidLow = extractDeleteKeyMidLow(deleteKey);
        int deleteLineMidLow = bitCount(deleteKeyMidLow);

        auto deleteKeyMidHigh = extractDeleteKeyMidHigh(deleteKey);
        int deleteLineMidHigh = bitCount(deleteKeyMidHigh);

        auto deleteKeyHigh = extractDeleteKeyHigh(deleteKey);

        int deleteLine1 = deleteLineLow;
        int deleteLine2 = deleteLineLow + deleteLineMidLow;
        int deleteLine3 = deleteLine2 + deleteLineMidHigh;

        if (deleteLine3 < 6) {
            // Low & MidLow & MidHigh & High
            int leftLine3 = 6 - deleteLine3;
            auto newXBoardHigh = insertWhiteLine(
                    (xBoardHigh_ << 10U * deleteLine3) |
                    ((xBoardMidHigh_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            int leftLine2 = 6 - deleteLine2;
            auto newXBoardMidHigh = insertWhiteLine(
                    (xBoardMidHigh_ << 10U * deleteLine2) |
                    ((xBoardMidLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
            );

            int leftLine1 = 6 - deleteLine1;
            auto newXBoardMidLow = insertWhiteLine(
                    (xBoardMidLow_ << 10U * deleteLine1) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
            );

            auto newXBoardLow = insertWhiteLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

            xBoardLow_ = newXBoardLow;
            xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
            xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
            xBoardHigh_ = newXBoardHigh & kValidBoardRange;
        } else if (deleteLine3 < 12) {
            // Low & MidLow & MidHigh
            int deleteLine3_6 = deleteLine3 - 6;
            int leftLine3 = 6 - deleteLine3_6;
            auto newXBoardHigh = insertWhiteLine(
                    (xBoardMidHigh_ << 10U * deleteLine3_6) |
                    ((xBoardMidLow_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            if (deleteLine2 < 6) {
                // Low & MidLow & MidHigh
                int leftLine2 = 6 - deleteLine2;
                auto newXBoardMidHigh = insertWhiteLine(
                        (xBoardMidHigh_ << 10U * deleteLine2) |
                        ((xBoardMidLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
                );

                int leftLine1 = 6 - deleteLine1;
                auto newXBoardMidLow = insertWhiteLine(
                        (xBoardMidLow_ << 10U * deleteLine1) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
                );

                auto newXBoardLow = insertWhiteLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

                xBoardLow_ = newXBoardLow;
                xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
                xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
                xBoardHigh_ = newXBoardHigh & kValidBoardRange;
            } else {
                // Low & MidLow
                int deleteLine2_6 = deleteLine2 - 6;
                int leftLine2 = 6 - deleteLine2_6;
                auto newXBoardMidHigh = insertWhiteLine(
                        (xBoardMidLow_ << 10U * deleteLine2_6) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
                );

                int leftLine1 = 6 - deleteLine1;
                auto newXBoardMidLow = insertWhiteLine(
                        (xBoardMidLow_ << 10U * deleteLine1) |
                        ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
                );

                auto newXBoardLow = insertWhiteLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

                xBoardLow_ = newXBoardLow;
                xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
                xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
                xBoardHigh_ = newXBoardHigh & kValidBoardRange;
            }
        } else {
            // Low & MidLow
            int deleteLine3_12 = deleteLine3 - 12;
            int leftLine3 = 6 - deleteLine3_12;
            auto newXBoardHigh = insertWhiteLine(
                    (xBoardMidLow_ << 10U * deleteLine3_12) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine3)) >> 10U * leftLine3), deleteKeyHigh
            );

            int deleteLine2_6 = deleteLine2 - 6;
            int leftLine2 = 6 - deleteLine2_6;
            auto newXBoardMidHigh = insertWhiteLine(
                    (xBoardMidLow_ << 10U * deleteLine2_6) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine2)) >> 10U * leftLine2), deleteKeyMidHigh
            );

            int leftLine1 = 6 - deleteLine1;
            auto newXBoardMidLow = insertWhiteLine(
                    (xBoardMidLow_ << 10U * deleteLine1) |
                    ((xBoardLow_ & getRowMaskAboveY(leftLine1)) >> 10U * leftLine1), deleteKeyMidLow
            );

            auto newXBoardLow = insertWhiteLine(xBoardLow_ & getRowMaskBelowY(leftLine1), deleteKeyLow);

            xBoardLow_ = newXBoardLow;
            xBoardMidLow_ = newXBoardMidLow & kValidBoardRange;
            xBoardMidHigh_ = newXBoardMidHigh & kValidBoardRange;
            xBoardHigh_ = newXBoardHigh & kValidBoardRange;
        }
    }

    bool Field::canMerge(const Field &other) const {
        return (xBoardLow_ & other.xBoardLow_) == 0L
               && (xBoardMidLow_ & other.xBoardMidLow_) == 0L
               && (xBoardMidHigh_ & other.xBoardMidHigh_) == 0L
               && (xBoardHigh_ & other.xBoardHigh_) == 0L;
    }

    void Field::merge(const Field &other) {
        xBoardLow_ |= other.xBoardLow_;
        xBoardMidLow_ |= other.xBoardMidLow_;
        xBoardMidHigh_ |= other.xBoardMidHigh_;
        xBoardHigh_ |= other.xBoardHigh_;
    }

    void Field::reduce(const Field &other) {
        xBoardLow_ &= ~other.xBoardLow_;
        xBoardMidLow_ &= ~other.xBoardMidLow_;
        xBoardMidHigh_ &= ~other.xBoardMidHigh_;
        xBoardHigh_ &= ~other.xBoardHigh_;
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
                char mark = marks.at((maxY - y - 1U) * 10 + x);
                if (mark != ' ' && mark != '_') {
                    field.setBlock(x, y);
                }
            }
        }

        return field;
    }
}
