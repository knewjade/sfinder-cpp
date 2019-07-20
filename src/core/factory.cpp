#include <cassert>

#include "factory.hpp"

namespace core {
    BlocksMask Blocks::mask(int leftX, int lowerY) const {
        assert(0 <= leftX && leftX <= kFieldWidth - width);
        assert(0 <= lowerY && lowerY < 6);

        if (6 < lowerY + height) {
            // Over
            const auto slide = mask_ << static_cast<unsigned int>(leftX);
            return {
                    (slide << (lowerY * kFieldWidthUnsigned)) & kValidBoardRange,
                    slide >> ((6 - lowerY) * kFieldWidthUnsigned)
            };
        } else {
            // Fit in the lower 6
            return {
                    mask_ << (lowerY * kFieldWidthUnsigned + leftX), 0
            };
        }
    }

    Collider Blocks::harddrop(int leftX, int lowerY) const {
        assert(0 <= leftX && leftX <= kFieldWidth - width);
        assert(0 <= lowerY && lowerY < kMaxFieldHeight);

        auto uLeftX = static_cast<unsigned int>(leftX);
        auto &collider = harddropColliders_[lowerY];
        return Collider{
                collider.boards[0] << uLeftX,
                collider.boards[1] << uLeftX,
                collider.boards[2] << uLeftX,
                collider.boards[3] << uLeftX,
        };
    }

    Factory Factory::create() {
        auto t = Piece::create<PieceType::T>();
        auto i = Piece::create<PieceType::I>();
        auto l = Piece::create<PieceType::L>();
        auto j = Piece::create<PieceType::J>();
        auto s = Piece::create<PieceType::S>();
        auto z = Piece::create<PieceType::Z>();
        auto o = Piece::create<PieceType::O>();

        return Factory(
                std::array<Piece, 7>{
                        t, i, l, j, s, z, o
                },
                std::array<Blocks, 4 * 7>{
                        t.blocks[RotateType::Spawn], t.blocks[RotateType::Right],
                        t.blocks[RotateType::Reverse], t.blocks[RotateType::Left],

                        i.blocks[RotateType::Spawn], i.blocks[RotateType::Right],
                        i.blocks[RotateType::Reverse], i.blocks[RotateType::Left],

                        l.blocks[RotateType::Spawn], l.blocks[RotateType::Right],
                        l.blocks[RotateType::Reverse], l.blocks[RotateType::Left],

                        j.blocks[RotateType::Spawn], j.blocks[RotateType::Right],
                        j.blocks[RotateType::Reverse], j.blocks[RotateType::Left],

                        s.blocks[RotateType::Spawn], s.blocks[RotateType::Right],
                        s.blocks[RotateType::Reverse], s.blocks[RotateType::Left],

                        z.blocks[RotateType::Spawn], z.blocks[RotateType::Right],
                        z.blocks[RotateType::Reverse], z.blocks[RotateType::Left],

                        o.blocks[RotateType::Spawn], o.blocks[RotateType::Right],
                        o.blocks[RotateType::Reverse], o.blocks[RotateType::Left],
                }
        );
    }

    const Piece &Factory::get(PieceType piece) const {
        return pieces[piece];
    }

    const Blocks &Factory::get(PieceType piece, RotateType rotate) const {
        int index = piece * 4 + rotate;
        assert(0 <= index && index < static_cast<int>(blocks.size()));
        return blocks[index];
    }
}
