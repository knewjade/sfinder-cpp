#include "perfect.hpp"

namespace finder {
    template<>
    bool PerfectFinder<core::srs::MoveGenerator>::search(
            Configure &configure,
            Candidate &candidate
    ) {
        auto maxDepth = configure.maxDepth;
        auto pieces = configure.pieces;
        auto leftLine = candidate.leftLine;
        auto moves = configure.movePool[candidate.depth];
        auto currentIndex = candidate.currentIndex;
        auto holdIndex = candidate.holdIndex;
        auto &field = candidate.field;

        if (currentIndex < configure.pieceSize) {
            auto &current = pieces[currentIndex];

            moves.clear();
            moveGenerator.search(moves, field, current, leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(current, move.rotateType);

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

                Candidate candidate = Candidate{freeze, currentIndex + 1, holdIndex, nextLeftLine, nextDepth};
                if (search(configure, candidate)) {
                    return true;
                }
            }
        }

        if (0 <= holdIndex) {
            // Hold exists
            if (pieces[currentIndex] != pieces[holdIndex]) {
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

                    Candidate candidate = Candidate{freeze, currentIndex + 1, currentIndex, nextLeftLine, nextDepth};
                    if (search(configure, candidate)) {
                        return true;
                    }
                }
            }
        } else {
            // Empty hold
            int nextIndex = currentIndex + 1;
            if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
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
            const core::Field &field, std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty
    ) {
        auto freeze = core::Field(field);

        std::vector<std::vector<core::Move>> movePool;
        for (int i = 0; i < maxDepth; ++i) {
            movePool.push_back(std::vector<core::Move>{});
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
