#include "perfect.hpp"

namespace finder {
    namespace {
        bool validate(const core::Field &field, int maxLine) {
            int sum = maxLine - field.getBlockOnX(0, maxLine);
            for (int x = 1; x < core::FIELD_WIDTH; x++) {
                int emptyCountInColumn = maxLine - field.getBlockOnX(x, maxLine);
                if (field.isWallBetween(x, maxLine)) {
                    if (sum % 4 != 0)
                        return false;
                    sum = emptyCountInColumn;
                } else {
                    sum += emptyCountInColumn;
                }
            }

            return sum % 4 == 0;
        }
    }

    template<>
    bool PerfectFinder<core::srs::MoveGenerator>::search(
            Configure &configure,
            Candidate &candidate
    ) {
        auto maxDepth = configure.maxDepth;
        auto pieces = configure.pieces;
        auto moves = configure.movePool[candidate.depth];

        auto leftLine = candidate.leftLine;
        assert(0 < leftLine);
        auto currentIndex = candidate.currentIndex;
        assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
        auto holdIndex = candidate.holdIndex;
        assert(-1 <= holdIndex && holdIndex < configure.pieceSize);

        auto &field = candidate.field;

        bool canUseCurrent = currentIndex < configure.pieceSize;
        if (canUseCurrent) {
            assert(pieces.at(currentIndex));
            auto &current = pieces[currentIndex];

            moves.clear();
            moveGenerator.search(moves, field, current, leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(current, move.rotateType);

                auto freeze = core::Field(field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                int nextLeftLine = leftLine - numCleared;
                if (freeze == core::Field{}) {
                    return true;
                }

                auto nextDepth = candidate.depth + 1;
                if (maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                Candidate candidate = Candidate{freeze, currentIndex + 1, holdIndex, nextLeftLine, nextDepth};
                if (search(configure, candidate)) {
                    return true;
                }
            }
        }

        if (0 <= holdIndex) {
            assert(holdIndex < configure.pieceSize);

            // Hold exists
            if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                assert(pieces.at(holdIndex));
                auto &hold = pieces[holdIndex];

                moves.clear();
                moveGenerator.search(moves, field, hold, leftLine);

                for (const auto &move : moves) {
                    auto &blocks = factory.get(hold, move.rotateType);

                    auto freeze = core::Field(field);
                    freeze.put(blocks, move.x, move.y);

                    int numCleared = freeze.clearLineReturnNum();

                    int nextLeftLine = leftLine - numCleared;
                    if (nextLeftLine == 0) {
                        return true;
                    }

                    auto nextDepth = candidate.depth + 1;
                    if (maxDepth <= nextDepth) {
                        continue;
                    }

                    if (!validate(freeze, nextLeftLine)) {
                        continue;
                    }

                    Candidate candidate = Candidate{freeze, currentIndex + 1, currentIndex, nextLeftLine, nextDepth};
                    if (search(configure, candidate)) {
                        return true;
                    }
                }
            }
        } else {
            assert(canUseCurrent);

            // Empty hold
            int nextIndex = currentIndex + 1;
            if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                assert(pieces.at(nextIndex));
                auto &next = pieces[nextIndex];

                moves.clear();
                moveGenerator.search(moves, field, next, leftLine);

                for (const auto &move : moves) {
                    auto &blocks = factory.get(next, move.rotateType);

                    auto freeze = core::Field(field);
                    freeze.put(blocks, move.x, move.y);

                    int numCleared = freeze.clearLineReturnNum();

                    int nextLeftLine = leftLine - numCleared;
                    if (nextLeftLine == 0) {
                        return true;
                    }

                    auto nextDepth = candidate.depth + 1;
                    if (maxDepth <= nextDepth) {
                        continue;
                    }

                    if (!validate(freeze, nextLeftLine)) {
                        continue;
                    }

                    Candidate candidate = Candidate{freeze, nextIndex + 1, currentIndex, nextLeftLine, nextDepth};
                    if (search(configure, candidate)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    template<>
    bool PerfectFinder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty
    ) {
        auto freeze = core::Field(field);

        std::vector<std::vector<core::Move>> movePool;
        for (int i = 0; i < maxDepth; ++i) {
            movePool.emplace_back(std::vector<core::Move>{});
        }

        Configure configure{
                pieces,
                movePool,
                maxDepth,
                static_cast<int>(pieces.size()),
        };

        if (holdEmpty) {
            Candidate candidate = Candidate{freeze, 0, -1, maxLine, 0};
            return search(configure, candidate);
        } else {
            Candidate candidate = Candidate{freeze, 1, 0, maxLine, 0};
            return search(configure, candidate);
        }
    }
}
