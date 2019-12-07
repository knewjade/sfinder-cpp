#ifndef FINDER_ALL_PCS_TYPES_HPP
#define FINDER_ALL_PCS_TYPES_HPP

#include "core/field.hpp"
#include "core/piece.hpp"

namespace sfinder::all_pcs {
    /**
     * Field indexes
     *
     *     | ..                         .. |
     * y=3 | 30 31 32 33 34 35 36 37 38 39 |
     *   2 | 20 21 22 23 24 25 26 27 28 29 |
     *   1 | 10 11 12 13 14 15 16 17 18 19 |
     *   0 |  0  1  2  3  4  5  6  7  8  9 |
     *     - -- -- -- -- -- -- -- -- -- -- -
     *     x= 0  1  2  3  4  5  6  7  8  9
     */

    /**
     * Key (deleteKey, usingKey), Line (deletedLine)
     *
     * bit is represented y-position of deleted/using line.
     * Mapping bit and y-position for `Key` is referred to `core::getBitKey()`
     *                                `Line` is `bit = 1 << y`
     */

    /**
     * Relative blocks
     *
     * `relativeBlocks` is bit string represented the position of the block relative in the vertical direction from `minFieldIndex`
     *
     * Example: J-Left, x=1, y=2, height=4 -> 0b1110001
     * _X_
     * _X_
     * XX_
     * ___
     */

    /**
     * Scaffold, Head
     *
     * Example: S-Spawn, x=1, y=1, deletedKey=1048576 -> Scaffold=[0,0,2,MAX], Head=[2,4,4,-1]
     * __HH_
     * __XX_
     * _H_S_
     * _XX__
     * _SS__
     *
     * Example: S-Spawn, x=1, y=1, deletedKey=1048576 -> Scaffold=[0,-1,MAX,MAX], Head=[2,3,-1,-1]
     * __H_
     * _HX_
     * _XX_
     * _SX_
     */

    using LineBit = unsigned int;

    struct Mino {
        core::PieceType pieceType;
        core::RotateType rotateType;
        int x;
        int y;
        core::LineKey deletedKey;
        LineBit deletedLine;
        core::LineKey usingKey;
        LineBit usingLine;
        core::Field minoField;
        std::array<int, 4> fieldIndexes;  // ascending order
        int minVerticalIndex;  // `x*height+y`
        uint64_t verticalRelativeBlock;
        std::array<LineBit, 4> usingLineEachY;  // padding 0 when no used
        std::array<int, 4> usingYs;  // ascending order // padding -1 when no used
        std::array<int, 4> scaffoldYs;  // [0]=minY, increase index as to the right // padding INT_MAX when no used
        std::array<int, 4> headYs;  // [0]=minY, increase index as to the right // padding -1 when no used
    };
}
#endif //FINDER_ALL_PCS_TYPES_HPP
