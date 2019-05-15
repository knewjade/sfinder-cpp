#ifndef CORE_PIECE_HPP
#define CORE_PIECE_HPP

#include <string>
#include <algorithm>
#include <array>
#include <cassert>

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
        static Blocks create(const RotateType rotateType, const std::array<Point, 4> &points);

        const RotateType rotateType;
        const std::array<Point, 4> points;
        const std::array<Collider, MAX_FIELD_HEIGHT> harddropColliders;
        const int minX;
        const int maxX;
        const int minY;
        const int maxY;
        const int width;
        const int height;

        BlocksMask mask(int leftX, int lowerY) const;

        Collider harddrop(int leftX, int lowerY) const;

    private:
        Blocks(const RotateType rotateType, const std::array<Point, 4> points, const Bitboard mask,
               const std::array<Collider, MAX_FIELD_HEIGHT> harddropColliders,
               const MinMax &minMaxX, const MinMax &minMaxY)
                : rotateType(rotateType), points(points), harddropColliders(harddropColliders),
                  minX(minMaxX.first), maxX(minMaxX.second), minY(minMaxY.first), maxY(minMaxY.second),
                  width(minMaxX.second - minMaxX.first + 1), height(minMaxY.second - minMaxY.first + 1), mask_(mask) {
        };

        const Bitboard mask_;  // Left align
    };

    class Piece {
    public:
        template<size_t N>
        static Piece create(
                const PieceType pieceType,
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
        const size_t offsetsSize;
        const std::array<Transform, 4> transforms;
        const int32_t uniqueRotateBit;

    private:
        Piece(
                const PieceType pieceType,
                const std::string name,
                const std::array<Blocks, 4> blocks,
                const std::array<Offset, 20> rightOffsets,
                const std::array<Offset, 20> leftOffsets,
                const size_t offsetsSize,
                const std::array<Transform, 4> transforms,
                const int32_t uniqueRotate
        ) : pieceType(pieceType), name(name), blocks(blocks), rightOffsets(rightOffsets), leftOffsets(leftOffsets),
            offsetsSize(offsetsSize),
            transforms(transforms), uniqueRotateBit(uniqueRotate) {
        };
    };

    class Factory {
    public:
        static Factory create();

        const Piece &get(PieceType piece) const;

        const Blocks &get(PieceType piece, RotateType rotate) const;

    private:
        Factory(const std::array<Piece, 7> pieces, const std::array<Blocks, 28> blocks)
                : pieces(pieces), blocks(blocks) {
        };

        const std::array<Piece, 7> pieces;
        const std::array<Blocks, 28> blocks;
    };
}

#endif //CORE_PIECE_HPP
