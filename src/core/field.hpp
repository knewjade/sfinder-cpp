#ifndef CORE_FIELD_HPP
#define CORE_FIELD_HPP

#include <cassert>
#include <cstdint>

#include "types.hpp"
#include "bits.hpp"
#include "piece.hpp"

namespace core {
    union Field {
    public:
        Bitboard boards[4];
        struct {
            Bitboard xBoardLow;
            Bitboard xBoardMidLow;
            Bitboard xBoardMidHigh;
            Bitboard xBoardHigh;
        };

        Field() : xBoardLow(0), xBoardMidLow(0), xBoardMidHigh(0), xBoardHigh(0) {};

        Field(uint64_t low, uint64_t midLow, uint64_t midHigh, uint64_t high)
                : xBoardLow(low), xBoardMidLow(midLow), xBoardMidHigh(midHigh), xBoardHigh(high) {};

        void setBlock(int x, int y);

        void removeBlock(int x, int y);

        bool isEmpty(int x, int y) const;

        void put(const Blocks &blocks, int x, int y);

        void putAtMaskIndex(const Blocks &blocks, int leftX, int lowerY);

        bool canPut(const Blocks &blocks, int x, int y) const;

        bool canPutAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) const;

        bool isOnGround(const Blocks &blocks, int x, int y);

        void clearLine();

        LineKey clearLineReturnKey();

        std::string toString(int height);

    private:
        void deleteLine_(LineKey low, LineKey midLow, LineKey midHigh, LineKey high);
    };

    int rotateRight(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    );

    int rotateLeft(
            const Field &field, const Piece &piece, RotateType fromRotate, RotateType toRotate, int fromX, int fromY
    );
}

#endif //CORE_FIELD_HPP
