#ifndef FINDER_ALL_PCS_MINOS_HPP
#define FINDER_ALL_PCS_MINOS_HPP

#include "core/factory.hpp"
#include "sfinder/combinations.hpp"
#include "types.hpp"

namespace sfinder::all_pcs {
    Mino createMino(const core::Blocks &blocks, int x, int y, core::LineKey deletedKey, int height) {
        auto field = core::Field();
        field.put(blocks, x, y);
        field.insertWhiteLineWithKey(deletedKey);

        auto usingKey = field.usingKey();

        // Parse to fieldIndexes, verticalIndexes, scaffoldYs, headYs
        auto fieldIndexes = std::array<int, 4>{};
        auto verticalIndexes = std::array<int, 4>{};

        constexpr int maxInt = std::numeric_limits<int>::max();
        auto scaffoldYs = std::array<int, 4>{maxInt, maxInt, maxInt, maxInt};
        auto headYs = std::array<int, 4>{-1, -1, -1, -1};

        {
            int index = 0;
            for (int iy = 0; iy < height; ++iy) {
                for (int ix = x + blocks.minX; ix <= x + blocks.maxX; ++ix) {
                    if (!field.isEmpty(ix, iy)) {
                        // fieldIndexes, verticalIndexes
                        fieldIndexes[index] = iy * core::kFieldWidth + ix;
                        verticalIndexes[index] = ix * height + iy;

                        index += 1;

                        // scaffoldYs, headYs
                        auto localX = ix - (x + blocks.minX);
                        assert(0 <= localX && localX < 4);

                        if (iy - 1 < scaffoldYs[localX]) {
                            scaffoldYs[localX] = iy - 1;
                        }

                        if (headYs[localX] < iy + 1) {
                            headYs[localX] = iy + 1;
                        }
                    }
                }
            }

            assert(index == 4);
        }
        std::sort(verticalIndexes.begin(), verticalIndexes.end());

        // Parse to minVerticalIndex, verticalRelativeBlock
        uint64_t verticalRelativeBlock = 0ULL;
        auto minVerticalIndex = verticalIndexes[0];
        for (int &verticalIndex : verticalIndexes) {
            auto relativeVerticalIndex = verticalIndex - minVerticalIndex;
            verticalRelativeBlock |= 1ULL << relativeVerticalIndex;
        }

        // Parse to usingYs, usingLine
        auto usingYs = std::array<int, 4>{-1, -1, -1, -1};
        LineBit usingLine = 0U;
        {
            int index = 0;
            for (int iy = 0; iy < height; ++iy) {
                auto bit = core::getBitKey(iy);
                if (0 < (usingKey & bit)) {
                    usingYs[index] = iy;
                    index += 1;

                    usingLine |= 1ULL << iy;
                }
            }
        }

        // Parse to deletedLine, usingLineEachY
        LineBit deletedLine = 0L;
        auto usingLineEachY = std::array<LineBit, 4>{0, 0, 0, 0};
        for (int iy = 0; iy < height; ++iy) {
            auto bit = core::getBitKey(iy);
            if (0 < (deletedKey & bit)) {
                deletedLine |= 1ULL << iy;
                usingLineEachY[iy] = usingLine;
            }
        }

        return Mino{
                blocks.pieceType, blocks.rotateType, x, y, deletedKey,
                deletedLine, usingKey, usingLine,
                field, fieldIndexes,
                minVerticalIndex, verticalRelativeBlock,
                usingLineEachY, usingYs,
                scaffoldYs, headYs,
        };
    }

    void putMinos(const core::Factory &factory, int width, int height, std::vector<Mino> &minos) {
        for (const auto &pieceType : core::kAllPieceType) {
            auto piece = factory.get(pieceType);

            auto bit = piece.uniqueShapeRotate;
            while (bit != 0) {
                auto rotateBit = bit & (-bit);
                auto rotateType = core::rotateBitToVal[rotateBit];

                auto blocks = factory.get(pieceType, rotateType);
                for (int y = -blocks.minY; y < height - blocks.maxY; ++y) {
                    auto deletedKeys = std::set<core::LineKey>{};

                    int emptyHeight = height - (y + blocks.maxY + 1);
                    for (int emptyLineCount = 0; emptyLineCount <= emptyHeight; ++emptyLineCount) {
                        auto emptyYs = std::vector<int>{};
                        for (int ey = y + blocks.minY + 1; ey <= y + blocks.maxY + emptyLineCount - 1; ++ey) {
                            emptyYs.push_back(ey);
                        }

                        if (emptyYs.empty() || emptyYs.size() < emptyLineCount) {
                            deletedKeys.insert(0ULL);
                        } else {
                            combination(emptyYs, emptyLineCount, [&](const std::vector<int> &selectedYs) {
                                core::LineKey key = 0ULL;
                                for (const auto &sy : selectedYs) {
                                    key |= core::getBitKey(sy);
                                }
                                deletedKeys.insert(key);
                            });
                        }
                    }

                    for (const auto deletedKey : deletedKeys) {
                        for (int x = -blocks.minX; x < width - blocks.maxX; ++x) {
                            minos.push_back(createMino(blocks, x, y, deletedKey, height));
                        }
                    }
                }

                bit = bit - rotateBit;
            }
        }

        std::sort(minos.begin(), minos.end(), [](const Mino &lhs, const Mino &rhs) {
            if (lhs.minVerticalIndex != rhs.minVerticalIndex) {
                return lhs.minVerticalIndex < rhs.minVerticalIndex;
            }

            if (lhs.deletedLine != rhs.deletedLine) {
                return lhs.deletedLine < rhs.deletedLine;
            }

            if (lhs.pieceType != rhs.pieceType) {
                return lhs.pieceType < rhs.pieceType;
            }

            if (lhs.rotateType != rhs.rotateType) {
                return lhs.rotateType < rhs.rotateType;
            }

            if (lhs.x != rhs.x) {
                return lhs.x < rhs.x;
            }

            if (lhs.y != rhs.y) {
                return lhs.y < rhs.y;
            }

            return false;
        });
    }
}

#endif //FINDER_ALL_PCS_MINOS_HPP
