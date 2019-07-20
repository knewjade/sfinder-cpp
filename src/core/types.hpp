#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <cstdint>
#include <string>
#include <array>

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

    const std::array<PieceType, 7> kAllPieceType = {
            PieceType::T,
            PieceType::I,
            PieceType::L,
            PieceType::J,
            PieceType::S,
            PieceType::Z,
            PieceType::O,
    };

    const std::string pieceNames[7]{
            std::string("T"),
            std::string("I"),
            std::string("L"),
            std::string("J"),
            std::string("S"),
            std::string("Z"),
            std::string("O"),
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
    constexpr unsigned int kFieldWidthUnsigned = kFieldWidth;

    constexpr auto kMaxFieldHeight = 24;
    constexpr uint64_t kValidBoardRange = 0xfffffffffffffffULL;
}

#endif //CORE_TYPES_HPP
