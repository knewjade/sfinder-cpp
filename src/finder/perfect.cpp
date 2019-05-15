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

        bool shouldUpdate(const Record &oldRecord, const Record &newRecord) {
            if (newRecord.softdropCount != oldRecord.softdropCount) {
                return newRecord.softdropCount < oldRecord.softdropCount;
            }

            if (newRecord.lineClearCount != oldRecord.lineClearCount) {
                return newRecord.lineClearCount < oldRecord.lineClearCount;
            }

            return newRecord.holdCount < oldRecord.holdCount;
        }

        bool isWorseThanBest(const Record &best, const Candidate &current) {
            return best.softdropCount < current.softdropCount || INT_MAX < current.lineClearCount;
        }
    }

    template<>
    void PerfectFinder<core::srs::MoveGenerator>::move(
            const Configure &configure,
            const Candidate &candidate,
            Solution &solution,
            std::vector<core::Move> &moves,
            core::PieceType pieceType,
            int nextIndex,
            int nextHoldIndex,
            int nextHoldCount
    );

    template<>
    void PerfectFinder<core::srs::MoveGenerator>::search(
            const Configure &configure,
            const Candidate &candidate,
            Solution &solution
    ) {
        if (isWorseThanBest(best, candidate)) {
            return;
        }

        auto depth = candidate.depth;

        auto pieces = configure.pieces;
        auto moves = configure.movePool[depth];

        auto currentIndex = candidate.currentIndex;
        assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
        auto holdIndex = candidate.holdIndex;
        assert(-1 <= holdIndex && holdIndex < configure.pieceSize);

        auto holdCount = candidate.holdCount;

        bool canUseCurrent = currentIndex < configure.pieceSize;
        if (canUseCurrent) {
            assert(currentIndex < pieces.size());
            auto &current = pieces[currentIndex];

            moves.clear();
            move(configure, candidate, solution, moves, current, currentIndex + 1, holdIndex, holdCount);
        }

        if (0 <= holdIndex) {
            assert(holdIndex < pieces.size());

            // Hold exists
            if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                auto &hold = pieces[holdIndex];

                moves.clear();
                move(configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex, holdCount + 1);
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
                move(configure, candidate, solution, moves, next, nextIndex + 1, currentIndex, holdCount + 1);
            }
        }
    }

    template<>
    void PerfectFinder<core::srs::MoveGenerator>::accept(const Record& record) {
        assert(1 <= best.solution.size());

        if (best.solution[0].x == -1 || shouldUpdate(best, record)) {
            best.solution = record.solution;
            best.softdropCount = record.softdropCount;
            best.holdCount = record.holdCount;
            best.lineClearCount = record.lineClearCount;
        }
    }
    
    template<>
    void PerfectFinder<core::srs::MoveGenerator>::move(
            const Configure &configure,
            const Candidate &candidate,
            Solution &solution,
            std::vector<core::Move> &moves,
            core::PieceType pieceType,
            int nextIndex,
            int nextHoldIndex,
            int nextHoldCount
    ) {
        auto depth = candidate.depth;
        auto maxDepth = configure.maxDepth;
        auto &field = candidate.field;

        auto leftLine = candidate.leftLine;
        assert(0 < leftLine);

        auto softdropCount = candidate.softdropCount;
        auto lineClearCount = candidate.lineClearCount;

        moveGenerator.search(moves, field, pieceType, leftLine);

        for (const auto &move : moves) {
            auto &blocks = factory.get(pieceType, move.rotateType);

            auto freeze = core::Field(field);
            freeze.put(blocks, move.x, move.y);

            int numCleared = freeze.clearLineReturnNum();

            solution[depth].pieceType = pieceType;
            solution[depth].rotateType = move.rotateType;
            solution[depth].x = move.x;
            solution[depth].y = move.y;

            int nextSoftdropCount = move.harddrop ? softdropCount : softdropCount + 1;
            int nextLineClearCount = 0 < numCleared ? lineClearCount + 1 : lineClearCount;

            int nextLeftLine = leftLine - numCleared;
            if (nextLeftLine == 0) {
                auto record = Record{solution, nextSoftdropCount, nextHoldCount, nextLineClearCount};
                accept(record);
                return;
            }

            auto nextDepth = depth + 1;
            if (maxDepth <= nextDepth) {
                continue;
            }

            if (!validate(freeze, nextLeftLine)) {
                continue;
            }

            Candidate nextCandidate = Candidate{
                    freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                    nextSoftdropCount, nextHoldCount, nextLineClearCount
            };
            search(configure, nextCandidate, solution);
        }
    }

    template<>
    Solution PerfectFinder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty
    ) {
        assert(1 <= maxDepth);

        // Copy field
        auto freeze = core::Field(field);

        // Initialize moves
        std::vector<std::vector<core::Move>> movePool(maxDepth);
        for (int index = 0; index < maxDepth; ++index) {
            movePool[index] = std::vector<core::Move>{};
        }

        // Initialize solution
        Solution solution(maxDepth);
        for (int index = 0; index < maxDepth; ++index) {
            solution[index] = Operation{
                    core::PieceType::T, core::RotateType::Spawn, -1, -1
            };
        }

        // Initialize configure
        const Configure configure{
                pieces,
                movePool,
                maxDepth,
                static_cast<int>(pieces.size()),
        };

        // Create candidate
        Candidate candidate = holdEmpty
                              ? Candidate{freeze, 0, -1, maxLine, 0, 0, 0, 0}
                              : Candidate{freeze, 1, 0, maxLine, 0, 0, 0, 0};

        // Create current record & best record
        best = Record{
                std::vector(solution),
                INT_MAX,
                INT_MAX,
                INT_MAX,
        };

        // Execute
        search(configure, candidate, solution);

        return best.solution[0].x == -1 ? kNoSolution : std::vector<Operation>(best.solution);
    }
}
