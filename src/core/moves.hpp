#ifndef CORE_MOVE_HPP
#define CORE_MOVE_HPP

#include <vector>

#include "field.hpp"
#include "srs.hpp"

namespace core {
    struct Move {
        RotateType rotateType;
        int x;
        int y;
        bool harddrop;
    };

    struct ScoredMove {
        Field field;
        Move move;
        int numCleared;
        int score;
    };

    enum MoveResults {
        No = 0,
        Harddrop = 1,
        Softdrop = 2,
    };

    struct TargetObject {
        const Field &field;
        const Piece &piece;
    };

    inline bool operator==(const Move &lhs, const Move &rhs) {
        return lhs.rotateType == rhs.rotateType && lhs.x == rhs.x && lhs.y == rhs.y && lhs.harddrop == rhs.harddrop;
    }

    inline bool operator!=(const Move &lhs, const Move &rhs) {
        return !(lhs == rhs);
    }

    class Cache {
    public:
        void visit(int x, int y, RotateType rotateType);

        [[nodiscard]] bool isVisit(int x, int y, RotateType rotateType) const;

        void found(int x, int y, RotateType rotateType);

        [[nodiscard]] bool isFound(int x, int y, RotateType rotateType) const;

        void visitPartially(int x, int y, RotateType rotateType);

        [[nodiscard]] bool isVisitPartially(int x, int y, RotateType rotateType) const;

        void push(int x, int y, RotateType rotateType);

        [[nodiscard]] bool isPushed(int x, int y, RotateType rotateType) const;

        template<bool AllowSoftdropTap>
        void clear() {
            for (auto &board: visitedBoard) {
                board = 0;
            }
            if constexpr (!AllowSoftdropTap) {
                for (auto &board: visitedPartiallyBoard) {
                    board = 0;
                }
            }
            for (auto &board: foundBoard) {
                board = 0;
            }
            for (auto &board: pushedBoard) {
                board = 0;
            }
        }

        template<bool AllowSoftdropTap>
        void resetTrail() {
            for (auto &board: visitedBoard) {
                board = 0;
            }
            if constexpr (!AllowSoftdropTap) {
                for (auto &board: visitedPartiallyBoard) {
                    board = 0;
                }
            }
        }

    private:
        Bitboard visitedBoard[4 * 4];
        Bitboard foundBoard[4 * 4];
        Bitboard visitedPartiallyBoard[4 * 4];
        Bitboard pushedBoard[4 * 4];
    };

    namespace harddrop {
        class MoveGenerator {
        public:
            explicit MoveGenerator(const Factory &factory) : factory(factory) {
            }

            void search(std::vector<Move> &moves, const Field &field, PieceType pieceType, int validHeight);

        private:
            const Factory &factory;
        };
    }

    namespace srs {
        enum From {
            None,
            Right,
            Left,
            Down,
        };

        template<bool Allow180 = false, bool AllowSoftdropTap = true>
        class MoveGenerator {
        public:
            explicit MoveGenerator(const Factory &factory) : factory(factory), cache(Cache()), appearY(-1) {
            }

            void search(std::vector<Move> &moves, const Field &field, PieceType pieceType, int validHeight) {
                appearY = validHeight;

                cache.clear<AllowSoftdropTap>();

                auto &piece = factory.get(pieceType);
                auto target = TargetObject{field, piece};

                for (int rotate = 0; rotate < 4; ++rotate) {
                    auto rotateType = static_cast<RotateType>(rotate);

                    auto &blocks = factory.get(pieceType, rotateType);
                    for (int x = -blocks.minX, maxX = FIELD_WIDTH - blocks.maxX; x < maxX; ++x) {
                        for (int y = validHeight - blocks.maxY - 1; -blocks.minY <= y; --y) {
                            if (field.canPut(blocks, x, y) && field.isOnGround(blocks, x, y)) {
                                auto result = check(target, blocks, x, y, From::None, true);
                                if (result != MoveResults::No) {
                                    cache.found(x, y, rotateType);

                                    auto &transform = piece.transforms[rotateType];
                                    RotateType newRotate = transform.toRotate;
                                    int newX = x + transform.offset.x;
                                    int newY = y + transform.offset.y;
                                    if (!cache.isPushed(newX, newY, newRotate)) {
                                        cache.push(newX, newY, newRotate);
                                        moves.push_back(Move{newRotate, newX, newY, result == MoveResults::Harddrop});
                                    }
                                }
                                cache.resetTrail<AllowSoftdropTap>();
                            }
                        }
                    }
                }
            }

            bool canReach(const Field &field, PieceType pieceType, RotateType rotateType, int x, int y,
                          int validHeight) {
                assert(field.canPut(factory.get(pieceType, rotateType), x, y));

                appearY = validHeight;

                cache.clear<AllowSoftdropTap>();

                auto &piece = factory.get(pieceType);

                auto &transform = piece.transforms[rotateType];
                auto currentRotateType = transform.toRotate;
                auto currentX = x + transform.offset.x;
                auto currentY = y + transform.offset.y;

                auto bit = static_cast<unsigned>(piece.sameShapeRotates[currentRotateType]);
                assert(bit != 0);

                auto target = TargetObject{field, piece};

                do {
                    auto next = bit & (bit - 1U);
                    RotateType nextRotateType = rotateBitToVal[bit & ~next];

                    auto &blocks = factory.get(pieceType, nextRotateType);

                    auto &nextTransform = piece.transforms[nextRotateType];

                    assert(currentRotateType == nextTransform.toRotate);

                    auto nextX = currentX - nextTransform.offset.x;
                    auto nextY = currentY - nextTransform.offset.y;

                    if (check(target, blocks, nextX, nextY, From::None, true)) {
                        return true;
                    }

                    bit = next;
                } while (bit != 0);

                return false;
            }

        private:
            const Factory &factory;

            Cache cache;
            int appearY;

            MoveResults checkLeftRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before left rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 1) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate90;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.offsetsSize; ++index) {
                    auto &offset = piece.leftOffsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::left(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.leftOffsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None, false);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults checkRightRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before right rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 3) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate90;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.offsetsSize; ++index) {
                    auto &offset = piece.rightOffsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.rightOffsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None, false);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults check180Rotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before 180 rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 2) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate180;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.rotate180OffsetsSize; ++index) {
                    auto &offset = piece.rotate180Offsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::rotate180(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.rotate180Offsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None, false);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults check(
                const TargetObject &targetObject, const Blocks &blocks, int x, int y, From from, bool isFirstCall
            ) {
                auto &field = targetObject.field;

                // When AllowSoftdropTap is false, vertical movement is only allowed down to the ground. (= can be moved with Harddrop).
                // In other words, upward movement is only allowed during continuous rise from the ground or on the ground.
                bool allowUp = true;
                if constexpr (!AllowSoftdropTap) {
                    allowUp = from == From::Down || field.isOnGround(blocks, x, y);
                }

                // When reach by harddrop
                if (allowUp && field.canReachOnHarddrop(blocks, x, y)) {
                    return isFirstCall ? MoveResults::Harddrop : MoveResults::Softdrop;
                }

                // When reach the top
                if (allowUp && appearY <= y) {
                    return MoveResults::Softdrop;
                }

                RotateType rotate = blocks.rotateType;

                // Since the possible operations differ depending on whether it's in the air or not,
                // the states are managed separately so that each state can be searched.
                if (allowUp) {
                    // Visited already
                    if (cache.isVisit(x, y, rotate)) {
                        // Return no when it has been visited and not found because it can be covered by other path
                        return cache.isFound(x, y, rotate) ? MoveResults::Softdrop : MoveResults::No;
                    }

                    cache.visit(x, y, rotate);
                } else {
                    // Visited already
                    if (cache.isVisitPartially(x, y, rotate)) {
                        // Return no when it has been visited and not found because it can be covered by other path
                        return cache.isFound(x, y, rotate) ? MoveResults::Softdrop : MoveResults::No;
                    }

                    cache.visitPartially(x, y, rotate);
                }

                // Move up
                int upY = y + 1;
                if (allowUp && upY < appearY && field.canPut(blocks, x, upY)) {
                    auto result = check(targetObject, blocks, x, upY, From::Down, false);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move left
                int leftX = x - 1;
                if (from != From::Left && -blocks.minX <= leftX && field.canPut(blocks, leftX, y)) {
                    auto result = check(targetObject, blocks, leftX, y, From::Right, false);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move right
                int rightX = x + 1;
                if (from != From::Right && rightX < FIELD_WIDTH - blocks.maxX && field.canPut(blocks, rightX, y)) {
                    auto result = check(targetObject, blocks, rightX, y, From::Left, false);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating right
                {
                    auto result = checkRightRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating left
                {
                    auto result = checkLeftRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating 180
                if constexpr (Allow180) {
                    auto result = check180Rotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                return MoveResults::No;
            }
        };
    }

    namespace srs_rotate_end {
        enum From {
            None,
            Right,
            Left,
            Down,
        };

        template<bool Allow180 = false, bool AllowSoftdropTap = true>
        class Reachable {
        public:
            explicit Reachable(const Factory &factory) : factory(factory), cache(Cache()), appearY(-1) {
            }

            bool checks(const Field &field, PieceType pieceType, RotateType rotateType, int x, int y, int validHeight) {
                assert(field.canPut(factory.get(pieceType, rotateType), x, y));

                appearY = validHeight;

                cache.clear<AllowSoftdropTap>();

                auto &piece = factory.get(pieceType);

                auto &transform = piece.transforms[rotateType];
                auto currentRotateType = transform.toRotate;
                auto currentX = x + transform.offset.x;
                auto currentY = y + transform.offset.y;

                auto bit = piece.sameShapeRotates[currentRotateType];
                assert(bit != 0);

                auto target = TargetObject{field, piece};

                do {
                    auto next = bit & (bit - 1);
                    RotateType nextRotateType = rotateBitToVal[bit & ~next];

                    auto &blocks = factory.get(pieceType, nextRotateType);

                    auto &nextTransform = piece.transforms[nextRotateType];

                    assert(currentRotateType == nextTransform.toRotate);

                    auto nextX = currentX - nextTransform.offset.x;
                    auto nextY = currentY - nextTransform.offset.y;

                    if (firstCheck(target, blocks, nextX, nextY)) {
                        return true;
                    }

                    bit = next;
                } while (bit != 0);

                return false;
            }

        private:
            const Factory &factory;

            Cache cache;
            int appearY;

            MoveResults checkLeftRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before left rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 1) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate90;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.offsetsSize; ++index) {
                    auto &offset = piece.leftOffsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::left(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.leftOffsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults checkRightRotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before right rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 3) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate90;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.offsetsSize; ++index) {
                    auto &offset = piece.rightOffsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.rightOffsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults check180Rotation(const TargetObject &targetObject, const Blocks &toBlocks, int toX, int toY) {
                auto &piece = targetObject.piece;
                auto &field = targetObject.field;

                // Direction before 180 rotation
                auto fromRotate = static_cast<RotateType>((toBlocks.rotateType + 2) % 4);
                auto &fromBlocks = piece.blocks[fromRotate];

                // Change the direction to `from`
                int toLeftX = toX + fromBlocks.minX;
                int toLowerY = toY + fromBlocks.minY;

                auto head = fromRotate * Piece::MaxOffsetRotate180;
                int width = FIELD_WIDTH - fromBlocks.width;
                for (int index = head; index < head + piece.rotate180OffsetsSize; ++index) {
                    auto &offset = piece.rotate180Offsets[index];
                    int fromLeftX = toLeftX - offset.x;
                    int fromLowerY = toLowerY - offset.y;
                    if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                        field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                        int fromX = toX - offset.x;
                        int fromY = toY - offset.y;
                        int srsResult = srs::rotate180(field, piece, fromRotate, toBlocks, fromX, fromY);
                        if (srsResult == -1) {
                            continue;
                        }

                        auto &kicks = piece.rotate180Offsets[srsResult];
                        if (offset.x == kicks.x && offset.y == kicks.y) {
                            auto result = check(targetObject, fromBlocks, fromX, fromY, From::None);
                            if (result != MoveResults::No) {
                                return result;
                            }
                        }
                    }
                }

                return MoveResults::No;
            }

            MoveResults firstCheck(const TargetObject &targetObject, const Blocks &blocks, int x, int y) {
                // Move the place where there is a possibility of rotating right
                {
                    auto result = checkRightRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating left
                {
                    auto result = checkLeftRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating 180
                if constexpr (Allow180) {
                    auto result = check180Rotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                return MoveResults::No;
            }

            MoveResults check(const TargetObject &targetObject, const Blocks &blocks, int x, int y, From from) {
                auto &field = targetObject.field;

                // When AllowSoftdropTap is false, vertical movement is only allowed down to the ground. (= can be moved with Harddrop).
                // In other words, upward movement is only allowed during continuous rise from the ground or on the ground.
                bool allowUp = true;
                if constexpr (!AllowSoftdropTap) {
                    allowUp = from == From::Down || field.isOnGround(blocks, x, y);
                }

                // When reach by harddrop
                if (allowUp && field.canReachOnHarddrop(blocks, x, y)) {
                    return MoveResults::Softdrop;
                }

                // When reach the top
                if (allowUp && appearY <= y) {
                    return MoveResults::Softdrop;
                }

                RotateType rotate = blocks.rotateType;

                // Since the possible operations differ depending on whether it's in the air or not,
                // the states are managed separately so that each state can be searched.
                if (allowUp) {
                    // Visited already
                    if (cache.isVisit(x, y, rotate)) {
                        // Return no when it has been visited and not found because it can be covered by other path
                        return cache.isFound(x, y, rotate) ? MoveResults::Softdrop : MoveResults::No;
                    }

                    cache.visit(x, y, rotate);
                } else {
                    // Visited already
                    if (cache.isVisitPartially(x, y, rotate)) {
                        // Return no when it has been visited and not found because it can be covered by other path
                        return cache.isFound(x, y, rotate) ? MoveResults::Softdrop : MoveResults::No;
                    }

                    cache.visitPartially(x, y, rotate);
                }

                // Move up
                int upY = y + 1;
                if (allowUp && upY < appearY && field.canPut(blocks, x, upY)) {
                    auto result = check(targetObject, blocks, x, upY, From::None);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move left
                int leftX = x - 1;
                if (from != From::Left && -blocks.minX <= leftX && field.canPut(blocks, leftX, y)) {
                    auto result = check(targetObject, blocks, leftX, y, From::Right);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move right
                int rightX = x + 1;
                if (from != From::Right && rightX < FIELD_WIDTH - blocks.maxX && field.canPut(blocks, rightX, y)) {
                    auto result = check(targetObject, blocks, rightX, y, From::Left);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating right
                {
                    auto result = checkRightRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating left
                {
                    auto result = checkLeftRotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                // Move the place where there is a possibility of rotating 180
                if constexpr (Allow180) {
                    auto result = check180Rotation(targetObject, blocks, x, y);
                    if (result != MoveResults::No) {
                        return result;
                    }
                }

                return MoveResults::No;
            }
        };
    }
}

#endif //CORE_MOVE_HPP
