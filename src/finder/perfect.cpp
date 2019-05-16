#include "perfect.hpp"

namespace finder {
    namespace {
        template<PriorityTypes T>
        bool shouldUpdate(const Record &oldRecord, const Record &newRecord);

        template<PriorityTypes T>
        bool isWorseThanBest(const Record &best, const Candidate &current);

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

        template<>
        bool shouldUpdate<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(
                const Record &oldRecord, const Record &newRecord
        ) {
            if (newRecord.softdropCount != oldRecord.softdropCount) {
                return newRecord.softdropCount < oldRecord.softdropCount;
            }

            if (newRecord.lineClearCount != oldRecord.lineClearCount) {
                return newRecord.lineClearCount < oldRecord.lineClearCount;
            }

            return newRecord.holdCount < oldRecord.holdCount;
        }

        template<>
        bool isWorseThanBest<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(
                const Record &best, const Candidate &current
        ) {
            // return best.softdropCount < current.softdropCount || INT_MAX < current.lineClearCount;
            return best.softdropCount < current.softdropCount;
        }

        template<>
        bool shouldUpdate<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(
                const Record &oldRecord, const Record &newRecord
        ) {
            if (newRecord.softdropCount != oldRecord.softdropCount) {
                return newRecord.softdropCount < oldRecord.softdropCount;
            }

            if (newRecord.maxCombo != oldRecord.maxCombo) {
                return oldRecord.maxCombo < newRecord.maxCombo;
            }

            if (newRecord.lineClearCount != oldRecord.lineClearCount) {
                return oldRecord.lineClearCount < newRecord.lineClearCount;
            }

            return newRecord.holdCount < oldRecord.holdCount;
        }

        template<>
        bool isWorseThanBest<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(
                const Record &best, const Candidate &current
        ) {
            return best.softdropCount < current.softdropCount;
        }

        bool shouldUpdate(const bool leastLineClears, const Record &oldRecord, const Record &newRecord) {
            if (leastLineClears) {
                return shouldUpdate<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(oldRecord, newRecord);
            } else {
                return shouldUpdate<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(oldRecord,
                                                                                                    newRecord);
            }
        }

        bool isWorseThanBest(const bool leastLineClears, const Record &best, const Candidate &current) {
            if (leastLineClears) {
                return isWorseThanBest<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(best, current);
            } else {
                return isWorseThanBest<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(best, current);
            }
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
        if (isWorseThanBest(configure.leastLineClears, best, candidate)) {
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
    void PerfectFinder<core::srs::MoveGenerator>::accept(const Configure &configure, const Record &record) {
        assert(1 <= best.solution.size());

        if (best.solution[0].x == -1 || shouldUpdate(configure.leastLineClears, best, record)) {
            best = Record(record);
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

        auto currentCombo = candidate.currentCombo;
        auto maxCombo = candidate.maxCombo;

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
            int nextCurrentCombo = 0 < numCleared ? currentCombo + 1 : 0;
            int nextMaxCombo = maxCombo < nextCurrentCombo ? nextCurrentCombo : maxCombo;

            int nextLeftLine = leftLine - numCleared;
            if (nextLeftLine == 0) {
                auto record = Record{
                        solution, nextSoftdropCount, nextHoldCount, nextLineClearCount, nextMaxCombo
                };
                accept(configure, record);
                return;
            }

            auto nextDepth = depth + 1;
            if (maxDepth <= nextDepth) {
                continue;
            }

            if (!validate(freeze, nextLeftLine)) {
                continue;
            }

            auto nextCandidate = Candidate{
                    freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                    nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo
            };
            search(configure, nextCandidate, solution);
        }
    }

    template<>
    Solution PerfectFinder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty, bool leastLineClears, int initCombo
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
                leastLineClears,
        };

        // Create candidate
        Candidate candidate = holdEmpty
                              ? Candidate{freeze, 0, -1, maxLine, 0, 0, 0, 0, initCombo, initCombo}
                              : Candidate{freeze, 1, 0, maxLine, 0, 0, 0, 0, initCombo, initCombo};

        // Create current record & best record
        best = Record{
                std::vector(solution),
                INT_MAX,
                INT_MAX,
                INT_MAX,
                0,
        };

        // Execute
        search(configure, candidate, solution);

        return best.solution[0].x == -1 ? kNoSolution : std::vector<Operation>(best.solution);
    }

    template<>
    Solution PerfectFinder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces,
            int maxDepth, int maxLine, bool holdEmpty
    ) {
        return run(field, pieces, maxDepth, maxLine, holdEmpty, false, 0);
    }
}
