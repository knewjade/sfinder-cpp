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
    bool PerfectFinder<core::srs::MoveGenerator>::move(
            Configure &configure,
            const Candidate &candidate,
            Solution &solution,
            std::vector<core::Move> &moves,
            core::PieceType pieceType,
            int nextIndex,
            int nextHoldIndex
    );

    template<>
    bool PerfectFinder<core::srs::MoveGenerator>::search(
            Configure &configure,
            const Candidate &candidate,
            Solution &solution
    ) {
        auto depth = candidate.depth;

        auto pieces = configure.pieces;
        auto moves = configure.movePool[depth];

        auto currentIndex = candidate.currentIndex;
        assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
        auto holdIndex = candidate.holdIndex;
        assert(-1 <= holdIndex && holdIndex < configure.pieceSize);

        bool canUseCurrent = currentIndex < configure.pieceSize;
        if (canUseCurrent) {
            assert(currentIndex < pieces.size());
            auto &current = pieces[currentIndex];

            moves.clear();
            if (move(configure, candidate, solution, moves, current, currentIndex + 1, holdIndex)) {
                return true;
            }
        }

        if (0 <= holdIndex) {
            assert(holdIndex < pieces.size());

            // Hold exists
            if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                auto &hold = pieces[holdIndex];

                moves.clear();
                if (move(configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex)) {
                    return true;
                }
            }
        } else {
            assert(canUseCurrent);

            // Empty hold
            int nextIndex = currentIndex + 1;
            assert(nextIndex < pieces.size() + 1);

            if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                assert(nextIndex < pieces.size());
                auto &next = pieces[nextIndex];

                moves.clear();
                if (move(configure, candidate, solution, moves, next, nextIndex + 1, currentIndex)) {
                    return true;
                }
            }
        }

        return false;
    }

    template<>
    bool PerfectFinder<core::srs::MoveGenerator>::move(
            Configure &configure,
            const Candidate &candidate,
            Solution &solution,
            std::vector<core::Move> &moves,
            core::PieceType pieceType,
            int nextIndex,
            int nextHoldIndex
    ) {
        auto depth = candidate.depth;
        auto maxDepth = configure.maxDepth;
        auto &field = candidate.field;

        auto leftLine = candidate.leftLine;
        assert(0 < leftLine);

        moveGenerator.search(moves, field, pieceType, leftLine);

        for (const auto &move : moves) {
            auto &blocks = factory.get(pieceType, move.rotateType);

            auto freeze = core::Field(field);
            freeze.put(blocks, move.x, move.y);

            int numCleared = freeze.clearLineReturnNum();

            int nextLeftLine = leftLine - numCleared;
            if (nextLeftLine == 0) {
                solution[depth].pieceType = pieceType;
                solution[depth].rotateType = move.rotateType;
                solution[depth].x = move.x;
                solution[depth].y = move.y;
                return true;
            }

            auto nextDepth = depth + 1;
            if (maxDepth <= nextDepth) {
                continue;
            }

            if (!validate(freeze, nextLeftLine)) {
                continue;
            }

            Candidate nextCandidate = Candidate{freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth};
            if (search(configure, nextCandidate, solution)) {
                solution[depth].pieceType = pieceType;
                solution[depth].rotateType = move.rotateType;
                solution[depth].x = move.x;
                solution[depth].y = move.y;
                return true;
            }
        }

        return false;
    }

    template<>
    Solution PerfectFinder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty
    ) {
        auto freeze = core::Field(field);

        std::vector<std::vector<core::Move>> movePool(maxDepth);
        Solution solution(maxDepth);
        for (int i = 0; i < maxDepth; ++i) {
            movePool[i] = std::vector<core::Move>{};
            solution[i] = Operation{};
        }

        Configure configure{
                pieces,
                movePool,
                maxDepth,
                static_cast<int>(pieces.size()),
        };

        Candidate candidate = holdEmpty
                              ? Candidate{freeze, 0, -1, maxLine, 0}
                              : Candidate{freeze, 1, 0, maxLine, 0};

        if (search(configure, candidate, solution)) {
            return solution;
        } else {
            return kNoSolution;
        }
    }
}
