#include "piece.hpp"

namespace core {
    namespace {
        const int MAX_FIELD_HEIGHT = 24;
        const uint64_t VALID_BOARD_RANGE = 0xfffffffffffffffL;

        std::array<Point, 4> rotateRight_(std::array<Point, 4> points) {
            return std::array<Point, 4>{
                    Point{points[0].y, -points[0].x},
                    Point{points[1].y, -points[1].x},
                    Point{points[2].y, -points[2].x},
                    Point{points[3].y, -points[3].x},
            };
        }

        std::array<Point, 4> rotateLeft_(std::array<Point, 4> points) {
            return std::array<Point, 4>{
                    Point{-points[0].y, points[0].x},
                    Point{-points[1].y, points[1].x},
                    Point{-points[2].y, points[2].x},
                    Point{-points[3].y, points[3].x},
            };
        }

        std::array<Point, 4> rotateReverse_(std::array<Point, 4> points) {
            return std::array<Point, 4>{
                    Point{-points[0].x, -points[0].y},
                    Point{-points[1].x, -points[1].y},
                    Point{-points[2].x, -points[2].y},
                    Point{-points[3].x, -points[3].y},
            };
        }

        uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            return 1LLU << (x + y * FIELD_WIDTH);
        }
    }

    Blocks Blocks::create(const RotateType rotateType, const std::array<Point, 4> &points) {
        MinMax minmaxX = std::minmax({points[0].x, points[1].x, points[2].x, points[3].x});
        MinMax minmaxY = std::minmax({points[0].y, points[1].y, points[2].y, points[3].y});

        // 左下揃えになるように移動
        Bitboard mask = 0;
        for (const auto &point : points) {
            mask |= getXMask(point.x - minmaxX.first, point.y - minmaxY.first);
        }

        return Blocks(rotateType, points, mask, minmaxX, minmaxY);
    }

    template<size_t N>
    Piece Piece::create(
            const PieceType pieceType,
            const std::string &name,
            const std::array<Point, 4> &points,
            const std::array<std::array<Offset, N>, 4> &offsets,
            const std::array<Transform, 4> &transforms
    ) {
        const Blocks &spawn = Blocks::create(RotateType::Spawn, points);
        const Blocks &right = Blocks::create(RotateType::Right, rotateRight_(points));
        const Blocks &reverse = Blocks::create(RotateType::Reverse, rotateReverse_(points));
        const Blocks &left = Blocks::create(RotateType::Left, rotateLeft_(points));

        std::array<Offset, 20> rightOffsets{};
        for (int rotate = 0; rotate < 4; ++rotate) {
            const auto &from = offsets[rotate];
            const auto &to = offsets[(rotate + 1) % 4];

            assert(from.size() == to.size());

            auto size = from.size();
            for (int index = 0; index < 5; ++index) {
                if (index < size) {
                    rightOffsets[rotate * 5 + index] = {from[index].x - to[index].x, from[index].y - to[index].y};
                } else {
                    rightOffsets[rotate * 5 + index] = {0, 0};
                }
            }
        }

        std::array<Offset, 20> leftOffsets{};
        for (int rotate = 0; rotate < 4; ++rotate) {
            const auto &from = offsets[rotate];
            const auto &to = offsets[(rotate + 3) % 4];

            assert(from.size() == to.size());

            auto size = from.size();
            for (int index = 0; index < 5; ++index) {
                if (index < size) {
                    leftOffsets[rotate * 5 + index] = {from[index].x - to[index].x, from[index].y - to[index].y};
                } else {
                    leftOffsets[rotate * 5 + index] = {0, 0};
                }
            }
        }

        int32_t uniqueRotate = 0;
        for (int rotate = 0; rotate < 4; ++rotate) {
            const auto &transform = transforms[rotate];
            uniqueRotate |= 1 << transform.toRotate;
        }

        return Piece(pieceType, name, std::array<Blocks, 4>{
                spawn, right, reverse, left
        }, rightOffsets, leftOffsets, N, transforms, uniqueRotate);
    }

    BlocksMask Blocks::mask(int leftX, int lowerY) const {
        if (6 < lowerY + height) {
            // 上にはみ出る
            const auto slide = mask_ << leftX;
            return {
                    (slide << (lowerY * FIELD_WIDTH)) & VALID_BOARD_RANGE, slide >> ((6 - lowerY) * FIELD_WIDTH)
            };
        } else {
            // 下6段で収まる
            return {
                    mask_ << (lowerY * FIELD_WIDTH + leftX), 0
            };
        }
    }

    Factory Factory::create() {
        using namespace std::literals::string_literals;

        auto io = std::array<std::array<Offset, 5>, 4>{
                std::array<Offset, 5>{Offset{0, 0}, Offset{-1, 0}, Offset{2, 0}, Offset{-1, 0}, Offset{2, 0}},
                std::array<Offset, 5>{Offset{-1, 0}, Offset{0, 0}, Offset{0, 0}, Offset{0, 1}, Offset{0, -2}},
                std::array<Offset, 5>{Offset{-1, 1}, Offset{1, 1}, Offset{-2, 1}, Offset{1, 0}, Offset{-2, 0}},
                std::array<Offset, 5>{Offset{0, 1}, Offset{0, 1}, Offset{0, 1}, Offset{0, -1}, Offset{0, 2}},
        };

        auto oo = std::array<std::array<Offset, 1>, 4>{
                std::array<Offset, 1>{Offset{0, 0}},
                std::array<Offset, 1>{Offset{0, -1}},
                std::array<Offset, 1>{Offset{-1, -1}},
                std::array<Offset, 1>{Offset{-1, 0}},
        };

        auto other = std::array<std::array<Offset, 5>, 4>{
                std::array<Offset, 5>{Offset{0, 0}, Offset{0, 0}, Offset{0, 0}, Offset{0, 0}, Offset{0, 0}},
                std::array<Offset, 5>{Offset{0, 0}, Offset{1, 0}, Offset{1, -1}, Offset{0, 2}, Offset{1, 2}},
                std::array<Offset, 5>{Offset{0, 0}, Offset{0, 0}, Offset{0, 0}, Offset{0, 0}, Offset{0, 0}},
                std::array<Offset, 5>{Offset{0, 0}, Offset{-1, 0}, Offset{-1, -1}, Offset{0, 2}, Offset{-1, 2}},
        };

        auto t = Piece::create(PieceType::T, "T"s, std::array<Point, 4>{
                Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{0, 1},
        }, other, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Right},
                Transform{Offset{0, 0}, RotateType::Reverse},
                Transform{Offset{0, 0}, RotateType::Left},
        });

        auto i = Piece::create(PieceType::I, "I"s, std::array<Point, 4>{
                Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{2, 0}
        }, io, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, -1}, RotateType::Left},
                Transform{Offset{-1, 0}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Left},
        });

        auto l = Piece::create(PieceType::L, "L"s, std::array<Point, 4>{
                Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{1, 1}
        }, other, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Right},
                Transform{Offset{0, 0}, RotateType::Reverse},
                Transform{Offset{0, 0}, RotateType::Left},
        });

        auto j = Piece::create(PieceType::J, "J"s, std::array<Point, 4>{
                Point{0, 0}, Point{-1, 0}, Point{1, 0}, Point{-1, 1}
        }, other, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Right},
                Transform{Offset{0, 0}, RotateType::Reverse},
                Transform{Offset{0, 0}, RotateType::Left},
        });

        auto s = Piece::create(PieceType::S, "S"s, std::array<Point, 4>{
                Point{0, 0}, Point{-1, 0}, Point{0, 1}, Point{1, 1}
        }, other, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{1, 0}, RotateType::Left},
                Transform{Offset{0, -1}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Left},
        });

        auto z = Piece::create(PieceType::Z, "Z"s, std::array<Point, 4>{
                Point{0, 0}, Point{1, 0}, Point{0, 1}, Point{-1, 1}
        }, other, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, 0}, RotateType::Right},
                Transform{Offset{0, -1}, RotateType::Spawn},
                Transform{Offset{-1, 0}, RotateType::Right},
        });

        auto o = Piece::create(PieceType::O, "O"s, std::array<Point, 4>{
                Point{0, 0}, Point{1, 0}, Point{0, 1}, Point{1, 1}
        }, oo, std::array<Transform, 4>{
                Transform{Offset{0, 0}, RotateType::Spawn},
                Transform{Offset{0, -1}, RotateType::Spawn},
                Transform{Offset{-1, -1}, RotateType::Spawn},
                Transform{Offset{-1, 0}, RotateType::Spawn},
        });

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
        assert(0 <= index && index < blocks.size());
        return blocks[index];
    }
}
