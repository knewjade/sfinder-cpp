#include <iostream>
#include "move.hpp"
#include "srs.hpp"

namespace core {
    namespace harddrop {
        void Searcher::search(
                std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
                int validHeight
        ) {
            auto &piece = factory.get(pieceType);

            auto bit = piece.uniqueRotateBit;
            assert(bit != 0);

            do {
                auto next = bit & (bit - 1);
                RotateType rotateType = rotateBitToVal[bit & ~next];

                auto &blocks = factory.get(pieceType, rotateType);

                int y = validHeight - blocks.minY;
                int maxY = validHeight - blocks.maxY;
                for (int x = -blocks.minX, maxX = FIELD_WIDTH - blocks.maxX; x < maxX; ++x) {
                    int harddropY = field.getYOnHarddrop(blocks, x, y);
                    if (harddropY < maxY) {
                        moves.push_back(Move{rotateType, x, harddropY});
                    }
                }

                bit = next;
            } while (bit != 0);
        }
    }

    namespace srs {
        namespace {
            const int MAX_FIELD_HEIGHT = 24;

            uint64_t getXMask(int x, int y) {
                assert(0 <= x && x < FIELD_WIDTH);
                assert(0 <= y && y < MAX_FIELD_HEIGHT);

                return 1LLU << (x + y * FIELD_WIDTH);
            }
        }

        void Searcher::search(
                std::vector<Move> &moves, const Factory &factory, const Field &field, const PieceType &pieceType,
                int validHeight
        ) {
            appearY = validHeight;

            auto &piece = factory.get(pieceType);
            cache.clear();

            for (int rotate = 0; rotate < 4; ++rotate) {
                RotateType rotateType = static_cast<RotateType >(rotate);

                auto &blocks = factory.get(pieceType, rotateType);
                for (int x = -blocks.minX, maxX = FIELD_WIDTH - blocks.maxX; x < maxX; ++x) {
                    for (int y = validHeight - blocks.maxY - 1; -blocks.minY <= y; --y) {
                        if (field.canPut(blocks, x, y) && field.isOnGround(blocks, x, y)) {
                            if (check(field, piece, blocks, x, y, From::None)) {
                                auto &transform = piece.transforms[rotateType];
                                RotateType newRotate = transform.toRotate;
                                int newX = x + transform.offset.x;
                                int newY = y + transform.offset.y;
                                if (!cache.isFound(newX, newY, newRotate)) {
                                    cache.found(newX, newY, newRotate);
                                    moves.push_back(Move{newRotate, newX, newY});
                                }
                            }
                            cache.resetTrail();
                        }
                    }
                }
            }
        }

        bool
        Searcher::checkLeftRotation(const Field &field, const Piece &piece, const Blocks &toBlocks, int toX, int toY) {
            // 左回転前の方向
            RotateType fromRotate = static_cast<RotateType>((toBlocks.rotateType + 1) % 4);
            auto &fromBlocks = piece.blocks[fromRotate];

            // 方向だけfromにする
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.leftOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY && field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int index = srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (index == -1) {
                        continue;
                    }

                    auto &kicks = piece.leftOffsets[index];
                    if (offset.x == kicks.x && offset.y == kicks.y) {
                        if (check(field, piece, fromBlocks, fromX, fromY, From::None)) {
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        bool Searcher::checkRightRotation(const Field &field, const Piece &piece, const Blocks &toBlocks, int toX, int toY) {
            // 右回転前の方向
            RotateType fromRotate = static_cast<RotateType>((toBlocks.rotateType + 3) % 4);
            auto &fromBlocks = piece.blocks[fromRotate];

            // 方向だけfromにする
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.rightOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY && field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int index = srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (index == -1) {
                        continue;
                    }

                    auto &kicks = piece.rightOffsets[index];
                    if (offset.x == kicks.x && offset.y == kicks.y) {
                        if (check(field, piece, fromBlocks, fromX, fromY, From::None)) {
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        bool Searcher::check(const Field &field, const Piece &piece, const Blocks &blocks, int x, int y, From from) {
            // 一番上までたどり着いたとき
            if (appearY <= y)
                return true;

            RotateType rotate = blocks.rotateType;

            // すでに訪問済みのとき
            if (cache.isVisit(x, y, rotate))
                return cache.isFound(x, y, rotate);  // その時の結果を返却。訪問済みだが結果が出てないときは他の探索でカバーできるためfalseを返却

            cache.visit(x, y, rotate);

            // harddropでたどりつけるとき
            if (field.canReachOnHarddrop(blocks, x, y)) {
                return true;
            }

            // 上に移動
            int upY = y + 1;
            if (upY < appearY && field.canPut(blocks, x, upY)) {
                if (check(field, piece, blocks, x, upY, From::None)) {
                    return true;
                }
            }

            // 左に移動
            int leftX = x - 1;
            if (from != From::Left && -blocks.minX <= leftX && field.canPut(blocks, leftX, y)) {
                if (check(field, piece, blocks, leftX, y, From::Right)) {
                    return true;
                }
            }

            // 右に移動
            int rightX = x + 1;
            if (from != From::Right && rightX < FIELD_WIDTH - blocks.maxX && field.canPut(blocks, rightX, y)) {
                if (check(field, piece, blocks, rightX, y, From::Left)) {
                    return true;
                }
            }

            // 右回転でくる可能性がある場所を移動
            if (checkRightRotation(field, piece, blocks, x, y)) {
                return true;
            }

            // 左回転でくる可能性がある場所を移動
            if (checkLeftRotation(field, piece, blocks, x, y)) {
                return true;
            }

            return false;
        }

        void Cache::visit(int x, int y, RotateType rotateType) {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            int index = y / 6;
            uint64_t mask = getXMask(x, y - 6 * index);

            int boardIndex = index + 4 * rotateType;
            visitedBoard[boardIndex] |= mask;
        }

        bool Cache::isVisit(int x, int y, core::RotateType rotateType) const {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            int index = y / 6;
            uint64_t mask = getXMask(x, y - 6 * index);

            int boardIndex = index + 4 * rotateType;
            return (visitedBoard[boardIndex] & mask) != 0;
        }

        void Cache::found(int x, int y, RotateType rotateType) {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            int index = y / 6;
            uint64_t mask = getXMask(x, y - 6 * index);

            int boardIndex = index + 4 * rotateType;
            foundBoard[boardIndex] |= mask;
        }

        bool Cache::isFound(int x, int y, core::RotateType rotateType) const {
            assert(0 <= x && x < FIELD_WIDTH);
            assert(0 <= y && y < MAX_FIELD_HEIGHT);

            int index = y / 6;
            uint64_t mask = getXMask(x, y - 6 * index);

            int boardIndex = index + 4 * rotateType;
            return (foundBoard[boardIndex] & mask) != 0;
        }

        void Cache::clear() {
            for (int index = 0; index < 16; ++index) {
                visitedBoard[index] = 0;
            }
            for (int index = 0; index < 16; ++index) {
                foundBoard[index] = 0;
            }
        }

        void Cache::resetTrail() {
            for (int index = 0; index < 16; ++index) {
                visitedBoard[index] = 0;
            }
        }
    }
}