#ifndef CORE_FIELD_HPP
#define CORE_FIELD_HPP

#include "types.hpp"
#include "bits.hpp"
#include "factory.hpp"

namespace core {
    union Field {
    public:
        Field() : Field(0ULL, 0ULL, 0ULL, 0ULL) {}

        Field(Bitboard xBoardLow) : Field(xBoardLow, 0ULL, 0ULL, 0ULL) {}

        Field(Bitboard xBoardLow, Bitboard xBoardMidLow) : Field(xBoardLow, xBoardMidLow, 0ULL, 0ULL) {}

        Field(Bitboard xBoardLow, Bitboard xBoardMidLow, Bitboard xBoardMidHigh)
                : Field(xBoardLow, xBoardMidLow, xBoardMidHigh, 0ULL) {}

        Field(Bitboard xBoardLow, Bitboard xBoardMidLow, Bitboard xBoardMidHigh, Bitboard xBoardHigh)
                : xBoardLow_(xBoardLow), xBoardMidLow_(xBoardMidLow),
                  xBoardMidHigh_(xBoardMidHigh), xBoardHigh_(xBoardHigh) {}

        void setBlock(int x, int y);

        void removeBlock(int x, int y);

        bool isEmpty(int x, int y) const;

        bool isEmptyOnY(int y) const;

        void put(const Blocks &blocks, int x, int y);

        void putAtMaskIndex(const Blocks &blocks, int leftX, int lowerY);

        void remove(const Blocks &blocks, int x, int y);

        void removeAtMaskIndex(const Blocks &blocks, int leftX, int lowerY);

        bool canPut(const Blocks &blocks, int x, int y) const;

        bool canPutAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) const;

        bool isOnGround(const Blocks &blocks, int x, int y) const;

        int getYOnHarddrop(const Blocks &blocks, int x, int startY) const;

        bool canReachOnHarddrop(const Blocks &blocks, int x, int y) const;

        void clearLine();

        int clearLineReturnNum();

        LineKey clearLineReturnKey();

        LineKey getUsingKey() const;

        int getBlockOnX(int ux, int maxY) const;

        bool isWallBetween(int x, int maxY) const;

        int numOfAllBlocks() const;

        void insertBlackLineWithKey(LineKey deleteKey);

        void insertWhiteLineWithKey(LineKey deleteKey);

        bool canMerge(const Field &other) const;

        bool contains(const Field &other) const;

        void merge(const Field &other);

        void reduce(const Field &other);

        std::string toString(int height) const;

        Bitboard boardLow() const {
            return xBoardLow_;
        }

        Bitboard boardMidLow() const {
            return xBoardMidLow_;
        }

        Bitboard boardMidHigh() const {
            return xBoardMidHigh_;
        }

        Bitboard boardHigh() const {
            return xBoardHigh_;
        }

    private:
        Bitboard boards_[4];
        struct {
            Bitboard xBoardLow_;
            Bitboard xBoardMidLow_;
            Bitboard xBoardMidHigh_;
            Bitboard xBoardHigh_;
        };

        void deleteLine_(LineKey low, LineKey midLow, LineKey midHigh, LineKey high);
    };

    inline bool operator==(const Field &lhs, const Field &rhs) {
        return lhs.boardLow() == rhs.boardLow() && lhs.boardMidLow() == rhs.boardMidLow()
               && lhs.boardMidHigh() == rhs.boardMidHigh() && lhs.boardHigh() == rhs.boardHigh();
    }

    inline bool operator!=(const Field &lhs, const Field &rhs) {
        return !(lhs == rhs);
    }

    Field createField(std::string marks);
}

#endif //CORE_FIELD_HPP
