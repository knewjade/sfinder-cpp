#ifndef CORE_FIELD_HPP
#define CORE_FIELD_HPP

#include <cassert>

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

        void setBlock(int x, int y);

        void removeBlock(int x, int y);

        bool isEmpty(int x, int y) const;

        void put(const Blocks &blocks, int x, int y);

        void putAtMaskIndex(const Blocks &blocks, int leftX, int lowerY);

        bool canPut(const Blocks &blocks, int x, int y) const;

        bool canPutAtMaskIndex(const Blocks &blocks, int leftX, int lowerY) const;

        bool isOnGround(const Blocks &blocks, int x, int y) const;

        int getYOnHarddrop(const Blocks &blocks, int x, int startY) const;

        bool canReachOnHarddrop(const Blocks &blocks, int x, int startY) const;

        void clearLine();

        LineKey clearLineReturnKey();

        std::string toString(int height);

    private:
        void deleteLine_(LineKey low, LineKey midLow, LineKey midHigh, LineKey high);
    };

    Field createField(std::string marks);
}

#endif //CORE_FIELD_HPP
