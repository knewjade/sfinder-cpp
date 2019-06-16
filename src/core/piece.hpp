#ifndef CORE_CPP_TEMPLATES_HPP
#define CORE_CPP_TEMPLATES_HPP

#include "types.hpp"

namespace core {
    struct Point {
        int x;
        int y;
    };

    struct Offset {
        int x;
        int y;
    };

    struct Transform {
        Offset offset;
        RotateType toRotate;
    };

    struct Collider {
        Bitboard boards[4];
    };

    // Offsets
    namespace {
        template<PieceType P>
        struct Offsets {
            static constexpr auto size = 5;

            static constexpr auto spawn = std::array<Offset, size>{Offset{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
            static constexpr auto right = std::array<Offset, size>{Offset{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}};
            static constexpr auto reverse = std::array<Offset, size>{Offset{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
            static constexpr auto left = std::array<Offset, size>{Offset{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}};
        };

        template<>
        struct Offsets<PieceType::I> {
            static constexpr auto size = 5;

            static constexpr auto spawn = std::array<Offset, size>{Offset{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}};
            static constexpr auto right = std::array<Offset, size>{Offset{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}};
            static constexpr auto reverse = std::array<Offset, size>{Offset{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}};
            static constexpr auto left = std::array<Offset, size>{Offset{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}};
        };

        template<>
        struct Offsets<PieceType::O> {
            static constexpr auto size = 1;

            static constexpr auto spawn = std::array<Offset, size>{Offset{0, 0}};
            static constexpr auto right = std::array<Offset, size>{Offset{0, -1}};
            static constexpr auto reverse = std::array<Offset, size>{Offset{-1, -1}};
            static constexpr auto left = std::array<Offset, size>{Offset{-1, 0}};
        };
    }

    // Transforms
    namespace {
        template<PieceType P>
        struct Transforms;

        template<>
        struct Transforms<PieceType::T> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, 0}, RotateType::Right};
            static constexpr auto reverse = Transform{Offset{0, 0}, RotateType::Reverse};
            static constexpr auto left = Transform{Offset{0, 0}, RotateType::Left};
        };

        template<>
        struct Transforms<PieceType::I> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, -1}, RotateType::Left};
            static constexpr auto reverse = Transform{Offset{-1, 0}, RotateType::Spawn};
            static constexpr auto left = Transform{Offset{0, 0}, RotateType::Left};
        };

        template<>
        struct Transforms<PieceType::L> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, 0}, RotateType::Right};
            static constexpr auto reverse = Transform{Offset{0, 0}, RotateType::Reverse};
            static constexpr auto left = Transform{Offset{0, 0}, RotateType::Left};
        };

        template<>
        struct Transforms<PieceType::J> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, 0}, RotateType::Right};
            static constexpr auto reverse = Transform{Offset{0, 0}, RotateType::Reverse};
            static constexpr auto left = Transform{Offset{0, 0}, RotateType::Left};
        };

        template<>
        struct Transforms<PieceType::S> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{1, 0}, RotateType::Left};
            static constexpr auto reverse = Transform{Offset{0, -1}, RotateType::Spawn};
            static constexpr auto left = Transform{Offset{0, 0}, RotateType::Left};
        };

        template<>
        struct Transforms<PieceType::Z> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, 0}, RotateType::Right};
            static constexpr auto reverse = Transform{Offset{0, -1}, RotateType::Spawn};
            static constexpr auto left = Transform{Offset{-1, 0}, RotateType::Right};
        };

        template<>
        struct Transforms<PieceType::O> {
            static constexpr auto spawn = Transform{Offset{0, 0}, RotateType::Spawn};
            static constexpr auto right = Transform{Offset{0, -1}, RotateType::Spawn};
            static constexpr auto reverse = Transform{Offset{-1, -1}, RotateType::Spawn};
            static constexpr auto left = Transform{Offset{-1, 0}, RotateType::Spawn};
        };
    }

    // Points
    namespace {
        template<PieceType P, RotateType R>
        struct Points {
        public:
            static constexpr auto leftRotateType = static_cast<core::RotateType>((R + 3) % 4);
            using LeftPoints = Points<P, leftRotateType>;

            static constexpr auto points() {
                return rotateRight(LeftPoints::points());
            }

        private:
            static constexpr auto rotateRight(std::array<Point, 4> points) {
                return std::array<Point, 4>{
                        Point{points[0].y, -points[0].x},
                        Point{points[1].y, -points[1].x},
                        Point{points[2].y, -points[2].x},
                        Point{points[3].y, -points[3].x},
                };
            }
        };

        template<>
        struct Points<PieceType::T, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {-1, 0}, {1, 0}, {0, 1}};
            }
        };

        template<>
        struct Points<PieceType::I, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {-1, 0}, {1, 0}, {2, 0}};
            }
        };

        template<>
        struct Points<PieceType::L, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {-1, 0}, {1, 0}, {1, 1}};
            }
        };

        template<>
        struct Points<PieceType::J, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {-1, 0}, {1, 0}, {-1, 1}};
            }
        };

        template<>
        struct Points<PieceType::S, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {-1, 0}, {0, 1}, {1, 1}};
            }
        };

        template<>
        struct Points<PieceType::Z, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {1, 0}, {0, 1}, {-1, 1}};
            }
        };

        template<>
        struct Points<PieceType::O, RotateType::Spawn> {
            static constexpr auto points() {
                return std::array<Point, 4>{Point{0, 0}, {1, 0}, {0, 1}, {1, 1}};
            }
        };
    }

    template<PieceType P>
    struct PieceBase {
    public:
        // types
        using transformsType = Transforms<P>;
        using offsetsType = Offsets<P>;

        // constexpr vars
        static constexpr auto transforms = std::array<Transform, 4>{
                transformsType::spawn, transformsType::right, transformsType::reverse, transformsType::left,
        };
        static constexpr auto offsets = std::array<std::array<Offset, offsetsType::size>, 4>{
                offsetsType::spawn, offsetsType::right, offsetsType::reverse, offsetsType::left,
        };

        // constexpr functions
        static constexpr std::array<Offset, 20> rightOffsets() {
            std::array<Offset, 20> rightOffsets{};
            for (int rotate = 0; rotate < 4; ++rotate) {
                const auto &from = offsets[rotate];
                const auto &to = offsets[(rotate + 1) % 4];

                assert(from.size() == to.size());

                auto size = from.size();
                for (int index = 0; index < 5; ++index) {
                    if (index < size) {
                        rightOffsets[rotate * 5 + index] = {from[index].x - to[index].x,
                                                            from[index].y - to[index].y};
                    } else {
                        rightOffsets[rotate * 5 + index] = {0, 0};
                    }
                }
            }
            return rightOffsets;
        }

        static constexpr std::array<Offset, 20> leftOffsets() {
            std::array<Offset, 20> leftOffsets{};
            for (int rotate = 0; rotate < 4; ++rotate) {
                const auto &from = offsets[rotate];
                const auto &to = offsets[(rotate + 3) % 4];

                assert(from.size() == to.size());

                auto size = from.size();
                for (int index = 0; index < 5; ++index) {
                    if (index < size) {
                        leftOffsets[rotate * 5 + index] = {from[index].x - to[index].x,
                                                           from[index].y - to[index].y};
                    } else {
                        leftOffsets[rotate * 5 + index] = {0, 0};
                    }
                }
            }
            return leftOffsets;
        }

        static constexpr uint8_t uniqueShapeRotate() {
            uint8_t uniqueRotate = 0;
            for (int rotate = 0; rotate < 4; ++rotate) {
                const auto &transform = transforms[rotate];
                uniqueRotate |= 1U << transform.toRotate;
            }
            return uniqueRotate;
        }

        static constexpr std::array<uint8_t, 4> sameShapeRotates() {
            // Find same shape rotate
            std::array<uint8_t, 4> sameShapeRotates{};
            for (int rotate = 0; rotate < 4; ++rotate) {
                uint8_t sameRotate = 0;
                for (unsigned int target = 0; target < 4; ++target) {
                    if (rotate == transforms[target].toRotate) {
                        sameRotate |= 1U << target;
                    }
                }
                sameShapeRotates[rotate] = sameRotate;
            }

            // Update all rotates that have the same shape
            for (int rotate = 0; rotate < 4; ++rotate) {
                core::RotateType afterRotate = transforms[rotate].toRotate;
                if (rotate != afterRotate) {
                    sameShapeRotates[rotate] = sameShapeRotates[afterRotate];
                }
            }

            return sameShapeRotates;
        }

        // methods
        static std::string name() {
            return pieceNames[P];
        }
    };

    template<PieceType P, RotateType R>
    struct BlocksBase {
    public:
        // types
        using pointsType = Points<P, R>;

        // constexpr vars
        static constexpr auto points = pointsType::points();

        static constexpr auto minX = std::min({points[0].x, points[1].x, points[2].x, points[3].x});
        static constexpr auto maxX = std::max({points[0].x, points[1].x, points[2].x, points[3].x});

        static constexpr auto minY = std::min({points[0].y, points[1].y, points[2].y, points[3].y});
        static constexpr auto maxY = std::max({points[0].y, points[1].y, points[2].y, points[3].y});

        // constexpr functions
        static constexpr Bitboard mask() {
            // Left align
            Bitboard mask{};
            for (const auto &point : points) {
                mask |= BlocksBase::getXMask(point.x - minX, point.y - minY);
            }
            return mask;
        }

        static constexpr std::array<Collider, kMaxFieldHeight> harddropColliders() {
            auto mask_ = mask();
            // Create colliders for harddrop
            std::array<Collider, kMaxFieldHeight> harddropColliders{};
            int height = maxY - minY + 1;
            int max = kMaxFieldHeight - height;
            harddropColliders[max] = mergeCollider(Collider{}, mask_, height, max);
            for (int index = max - 1; 0 <= index; --index) {
                harddropColliders[index] = mergeCollider(harddropColliders[index + 1], mask_, height, index);
            }
            return harddropColliders;
        }

    private:
        // constexpr functions
        static constexpr uint64_t getXMask(int x, int y) {
            assert(0 <= x && x < kFieldWidth);
            assert(0 <= y && y < kMaxFieldHeight);

            return 1LLU << (x + y * kFieldWidthUnsigned);
        }

        static constexpr Collider mergeCollider(
                const Collider &prev, const Bitboard mask, int height, int lowerY
        ) {
            auto collider = Collider{prev};
            assert(0 <= lowerY && lowerY + height <= kMaxFieldHeight);

            int index = lowerY / 6;
            int localY = lowerY - 6 * index;
            if (6 < localY + height) {
                // Over
                collider.boards[index] |= (mask << (localY * kFieldWidthUnsigned)) & kValidBoardRange;
                collider.boards[index + 1] |= mask >> ((6 - localY) * kFieldWidthUnsigned);
            } else {
                // Fit in the lower 6
                collider.boards[index] |= mask << (localY * kFieldWidthUnsigned);
            }

            return collider;
        }
    };
}

#endif //CORE_CPP_TEMPLATES_HPP
