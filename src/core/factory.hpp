#ifndef CORE_PIECE_HPP
#define CORE_PIECE_HPP

#include <algorithm>
#include <array>
#include <string>

#include "types.hpp"

namespace core {
    using MinMax = std::pair<int, int>;

    struct Point {
        int x;
        int y;
    };

    struct Offset {
        int x;
        int y;
    };

    struct Collider {
        Bitboard boards[4];
    };

    struct BlocksMask {
        Bitboard low;
        Bitboard high;
    };

    struct Transform {
        Offset offset;
        RotateType toRotate;
    };

    class Blocks {
    public:
        static Blocks create(RotateType rotateType, const std::array<Point, 4> &points);

        const RotateType rotateType;
        const int minX;
        const int maxX;
        const int minY;
        const int maxY;
        const int width;
        const int height;

        BlocksMask mask(int leftX, int lowerY) const;

        Collider harddrop(int leftX, int lowerY) const;

    private:
        Blocks(const RotateType rotateType, const MinMax &minMaxX, const MinMax &minMaxY,
               const std::array<Collider, kMaxFieldHeight> harddropColliders, const Bitboard mask)
                : rotateType(rotateType),
                  minX(minMaxX.first), maxX(minMaxX.second),
                  minY(minMaxY.first), maxY(minMaxY.second),
                  width(minMaxX.second - minMaxX.first + 1), height(minMaxY.second - minMaxY.first + 1),
                  harddropColliders_(harddropColliders), mask_(mask) {
        };

        const std::array<Collider, kMaxFieldHeight> harddropColliders_;
        const Bitboard mask_;  // Left align
    };

    class Piece {
    public:
        static constexpr size_t kAllOffsetsSize = 20;

        template <size_t N>
        static Piece create(
                PieceType pieceType,
                const std::string &name,
                const std::array<Point, 4> &points,
                const std::array<std::array<Offset, N>, 4> &offsets,
                const std::array<Transform, 4> &transforms
        );

        const PieceType pieceType;
        const std::string name;
        const std::array<Blocks, 4> blocks;
        const std::array<Offset, 20> rightOffsets;
        const std::array<Offset, 20> leftOffsets;
        const std::array<Transform, 4> transforms;
        const uint8_t uniqueRotateBit;
        const std::array<uint8_t, 4> sameShapeRotates;

    private:
        Piece(
                const PieceType pieceType,
                std::string name,
                const std::array<Blocks, 4> blocks,
                const std::array<Offset, 20> rightOffsets,
                const std::array<Offset, 20> leftOffsets,
                const std::array<Transform, 4> transforms,
                const uint8_t uniqueRotate,
                const std::array<uint8_t, 4> sameShapeRotates
        ) : pieceType(pieceType), name(std::move(name)), blocks(blocks),
            rightOffsets(rightOffsets), leftOffsets(leftOffsets), transforms(transforms),
            uniqueRotateBit(uniqueRotate), sameShapeRotates(sameShapeRotates) {
        };
    };

    class Factory {
    public:
        static Factory create();

        const Piece &get(PieceType piece) const;

        const Blocks &get(PieceType piece, RotateType rotate) const;

    private:
        Factory(std::array<Piece, 7> pieces, const std::array<Blocks, 28> &blocks)
                : pieces(std::move(pieces)), blocks(blocks) {
        };

        const std::array<Piece, 7> pieces;
        const std::array<Blocks, 28> blocks;
    };
}

#endif //CORE_PIECE_HPP
