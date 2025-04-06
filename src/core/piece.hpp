#ifndef CORE_PIECE_HPP
#define CORE_PIECE_HPP

#include <string>
#include <algorithm>
#include <array>

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

    inline Offset operator+(const Offset &lhs, const Offset &rhs) {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    inline Offset operator-(const Offset &lhs, const Offset &rhs) {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }

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
        static constexpr int MaxOffsetRotate90 = 5;
        static constexpr int MaxOffsetRotate180 = 6;

        template<size_t OffsetSizeRotate90>
        static Piece create(
                PieceType pieceType,
                const std::string &name,
                const std::array<Point, 4> &points,
                const std::array<std::array<Offset, OffsetSizeRotate90>, 4> &offsets,
                const std::array<Transform, 4> &transforms
        );

        template<size_t OffsetSizeRotate90, size_t OffsetSizeRotate180>
        static Piece create(
                PieceType pieceType,
                const std::string &name,
                const std::array<Point, 4> &points,
                const std::array<std::array<Offset, OffsetSizeRotate90>, 4> &offsets,
                const std::array<Offset, 24> &rotate180Offsets,
                const std::array<Transform, 4> &transforms
        );

        template <size_t OffsetSizeRotate90, size_t OffsetSizeRotate180>
        static Piece create(
            PieceType pieceType,
            const std::string &name,
            const std::array<Point, 4> &points,
            const std::array<Offset, MaxOffsetRotate90 * 4> &cwOffsets,
            const std::array<Offset, MaxOffsetRotate90 * 4> &ccwOffsets,
            const std::array<Offset, MaxOffsetRotate180 * 4> &rotate180Offsets,
            const std::array<Transform, 4> &transforms
        );

        const PieceType pieceType;
        const std::string name;
        const std::array<Blocks, 4> blocks;
        const std::array<Offset, MaxOffsetRotate90 * 4> rightOffsets; // = cwOffsets
        const std::array<Offset, MaxOffsetRotate90 * 4> leftOffsets; // = ccwOffsets
        const std::array<Offset, MaxOffsetRotate180 * 4> rotate180Offsets;
        const size_t offsetsSize;
        const size_t rotate180OffsetsSize;
        const std::array<Transform, 4> transforms;
        const int32_t uniqueRotateBit;
        const std::array<int32_t, 4> sameShapeRotates;

    private:
        Piece(
                const PieceType pieceType,
                const std::string name,
                const std::array<Blocks, 4> blocks,
                const std::array<Offset, 20> cwOffsets,
                const std::array<Offset, 20> ccwOffsets,
                const std::array<Offset, 24> rotate180Offsets,
                const size_t offsetsSize,
                const size_t rotate180OffsetsSize,
                const std::array<Transform, 4> transforms,
                const int32_t uniqueRotate,
                const std::array<int32_t, 4> sameShapeRotates
        ) : pieceType(pieceType), name(name), blocks(blocks),
            rightOffsets(cwOffsets), leftOffsets(ccwOffsets), rotate180Offsets(rotate180Offsets),
            offsetsSize(offsetsSize), rotate180OffsetsSize(rotate180OffsetsSize),
            transforms(transforms), uniqueRotateBit(uniqueRotate),
            sameShapeRotates(sameShapeRotates) {
        }
    };

    class Factory {
    public:
        static Factory create();

        static Factory createForSSRPlus();

        static Factory create(
            const Piece& t,
            const Piece& i,
            const Piece& l,
            const Piece& j,
            const Piece& s,
            const Piece& z,
            const Piece& o
        );

        const Piece &get(PieceType piece) const;

        const Blocks &get(PieceType piece, RotateType rotate) const;

    private:
        Factory(const std::array<Piece, 7> &pieces, const std::array<Blocks, 28> &blocks) : pieces(pieces), blocks(blocks) {
        };

        const std::array<Piece, 7> pieces;
        const std::array<Blocks, 28> blocks;
    };
}

#endif //CORE_PIECE_HPP
