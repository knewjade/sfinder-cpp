#ifndef CORE_BITS_HPP
#define CORE_BITS_HPP

#include "types.hpp"

namespace core {
    Bitboard deleteLine_(Bitboard x, LineKey key);

    Bitboard deleteLine(Bitboard x, LineKey mask);

    Bitboard insertBlackLine_(Bitboard x, LineKey key);

    Bitboard insertBlackLine(Bitboard x, LineKey mask);

    Bitboard insertWhiteLine_(Bitboard x, LineKey key);

    Bitboard insertWhiteLine(Bitboard x, LineKey mask);

    Bitboard getColumnOneLineBelowY(int maxY);

    bool isWallBetweenLeft(int x, int maxY, Bitboard board);

    int bitCount(uint64_t b);

    LineKey getBitKey(int y);

    LineKey getUsingLineKey(Bitboard board);

    Bitboard getRowMaskBelowY(int excludeY);

    Bitboard getRowMaskAboveY(int includeY);

    LineKey getUsingKeyBelowY(int excludeY);

    Bitboard getColumnMaskLeftX(int excludeX);

    Bitboard getColumnMaskRightX(int includeX);
}

#endif //CORE_BITS_HPP
