#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <cstdint>

namespace core {
    using Bitboard = uint64_t;
    using LineKey = uint64_t;

    enum PieceType {
        T = 0,
        I = 1,
        L = 2,
        J = 3,
        S = 4,
        Z = 5,
        O = 6,
    };

    enum RotateType {
        Spawn = 0,
        Right = 1,
        Reverse = 2,
        Left = 3,
    };

    constexpr auto dummyRotate = RotateType::Spawn;
    const RotateType rotateBitToVal[16]{dummyRotate,
                                        RotateType::Spawn,
                                        RotateType::Right, dummyRotate,
                                        RotateType::Reverse, dummyRotate, dummyRotate, dummyRotate,
                                        RotateType::Left,
    };

    constexpr auto kFieldWidth = 10;
    constexpr auto kMaxFieldHeight = 24;
}

#endif //CORE_TYPES_HPP
