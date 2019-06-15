#ifndef CORE_PIECE_HPP
#define CORE_PIECE_HPP

#include <algorithm>
#include <array>
#include <string>

#include "types.hpp"
#include "templates.hpp"

namespace core {
    using MinMax = std::pair<int, int>;

    struct BlocksMask {
        Bitboard low;
        Bitboard high;
    };

    class Blocks {
    public:
        template<PieceType P, RotateType R>
        static Blocks create() {
            using base = BlocksBase<P, R>;
            return Blocks(
                    R, base::minX, base::maxX, base::minY, base::maxY, base::harddropColliders(), base::mask()
            );
        }

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
        Blocks(
                const RotateType rotateType, const int minX, const int maxX, const int minY, const int maxY,
                const std::array<Collider, kMaxFieldHeight> harddropColliders, const Bitboard mask
        ) : rotateType(rotateType),
            minX(minX), maxX(maxX), minY(minY), maxY(maxY), width(maxX - minX + 1), height(maxY - minY + 1),
            harddropColliders_(harddropColliders), mask_(mask) {
        };

        const std::array<Collider, kMaxFieldHeight> harddropColliders_;
        const Bitboard mask_;  // Left align
    };

    class Piece {
    public:
        static constexpr int kOffsetsSize = 5;

        template<PieceType P>
        static Piece create() {
            const auto &spawn = Blocks::create<P, RotateType::Spawn>();
            const auto &right = Blocks::create<P, RotateType::Right>();
            const auto &reverse = Blocks::create<P, RotateType::Reverse>();
            const auto &left = Blocks::create<P, RotateType::Left>();

            using base = PieceBase<P>;
            return Piece(
                    P, base::name(),
                    std::array<Blocks, 4>{spawn, right, reverse, left},
                    base::rightOffsets(), base::leftOffsets(), base::transforms,
                    base::uniqueRotate(), base::sameShapeRotates()
            );
        }

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
