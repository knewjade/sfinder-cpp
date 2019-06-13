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
        bool isWorseThanBest<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(
                const Record &best, const Candidate &current
        ) {
            // return best.softdropCount < current.softdropCount || INT_MAX < current.lineClearCount;
            return best.softdropCount < current.softdropCount;
        }

        template<>
        bool shouldUpdate<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(
                const Record &oldRecord, const Record &newRecord
        ) {
            if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
                return oldRecord.tSpinAttack < newRecord.tSpinAttack;
            }

            if (newRecord.softdropCount != oldRecord.softdropCount) {
                return newRecord.softdropCount < oldRecord.softdropCount;
            }

            if (newRecord.lineClearCount != oldRecord.lineClearCount) {
                return newRecord.lineClearCount < oldRecord.lineClearCount;
            }

            return newRecord.holdCount < oldRecord.holdCount;
        }

        template<>
        bool isWorseThanBest<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(
                const Record &best, const Candidate &current
        ) {
            return best.softdropCount < current.softdropCount;
        }

        template<>
        bool shouldUpdate<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(
                const Record &oldRecord, const Record &newRecord
        ) {
            if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
                return oldRecord.tSpinAttack < newRecord.tSpinAttack;
            }

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

        bool shouldUpdate(const bool leastLineClears, const Record &oldRecord, const Record &newRecord) {
            if (leastLineClears) {
                return shouldUpdate<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(oldRecord, newRecord);
            } else {
                return shouldUpdate<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(oldRecord,
                                                                                                    newRecord);
            }
        }

        bool isWorseThanBest(const bool leastLineClears, const Record &best, const Candidate &current) {
            if (current.leftNumOfT == 0) {
                if (current.tSpinAttack != best.tSpinAttack) {
                    return current.tSpinAttack < best.tSpinAttack;
                }

                if (leastLineClears) {
                    return isWorseThanBest<PriorityTypes::LeastSoftdrop_LeastLineClear_LeastHold>(best, current);
                } else {
                    return isWorseThanBest<PriorityTypes::LeastSoftdrop_MostCombo_MostLineClear_LeastHold>(best, current);
                }
            }

            return false;
        }

        constexpr int FIELD_WIDTH = 10;
        constexpr int FIELD_HEIGHT = 24;

        bool isBlock(const core::Field &field, int x, int y) {
            if (x < 0 || FIELD_WIDTH <= x || y < 0) {
                return true;
            }
            return !field.isEmpty(x, y);
        }
    }

    TSpinShapes getTSpinShape(const core::Field &field, int x, int y, core::RotateType rotateType) {
        assert(0 <= x && x < FIELD_WIDTH);
        assert(0 <= y);

        auto b1 = isBlock(field, x - 1, y - 1);
        auto b2 = isBlock(field, x - 1, y + 1);
        auto b3 = isBlock(field, x + 1, y - 1);
        auto b4 = isBlock(field, x + 1, y + 1);

        auto shape = (b1 || b2) && (b1 || b3) && (b1 || b4) && (b2 || b3) && (b2 || b4) && (b3 || b4);
        if (!shape) {
            return TSpinShapes::NoShape;
        }

        switch (rotateType) {
            case core::RotateType::Spawn:
                return b1 && b3 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Right:
                return b1 && b2 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Reverse:
                return b2 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
            case core::RotateType::Left:
                return b3 && b4 ? TSpinShapes::MiniOrTSTShape : TSpinShapes::RegularShape;
        }

        assert(false);
        return TSpinShapes::NoShape;
    }

    int getAttackIfTSpin(
            core::srs_rotate_end::Reachable &reachable, const core::Factory &factory, const core::Field &field,
            core::PieceType pieceType, const core::Move &move, int numCleared, bool b2b
    ) {
        if (pieceType != core::PieceType::T) {
            return 0;
        }

        if (numCleared == 0) {
            return 0;
        }

        auto rotateType = move.rotateType;
        auto shapes = getTSpinShape(field, move.x, move.y, rotateType);
        if (shapes == TSpinShapes::NoShape) {
            return 0;
        }

        if (!reachable.checks(field, pieceType, rotateType, move.x, move.y, FIELD_HEIGHT)) {
            return 0;
        }

        if (shapes == TSpinShapes::RegularShape) {
            int baseAttack = numCleared * 2;
            return b2b ? baseAttack + 1 : baseAttack;
        }

        // Checks mini or regular (Last SRS test pattern)

        auto &piece = factory.get(pieceType);
        auto &toBlocks = factory.get(pieceType, rotateType);

        auto toX = move.x;
        auto toY = move.y;

        // Rotate right
        {
            // Direction before right rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 3) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (unsigned int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.rightOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                    field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int srsResult = core::srs::right(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (0 <= srsResult && srsResult % 5 == 4) {
                        // T-Spin Regular
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }

                    // Mini or No T-Spin
                }
            }
        }

        // Rotate left
        {
            // Direction before left rotation
            auto fromRotate = static_cast<core::RotateType>((rotateType + 1) % 4);
            auto &fromBlocks = factory.get(pieceType, fromRotate);

            // Change the direction to `from`
            int toLeftX = toX + fromBlocks.minX;
            int toLowerY = toY + fromBlocks.minY;

            auto head = fromRotate * 5;
            int width = FIELD_WIDTH - fromBlocks.width;
            for (unsigned int index = head; index < head + piece.offsetsSize; ++index) {
                auto &offset = piece.leftOffsets[index];
                int fromLeftX = toLeftX - offset.x;
                int fromLowerY = toLowerY - offset.y;
                if (0 <= fromLeftX && fromLeftX <= width && 0 <= fromLowerY &&
                    field.canPutAtMaskIndex(fromBlocks, fromLeftX, fromLowerY)) {
                    int fromX = toX - offset.x;
                    int fromY = toY - offset.y;
                    int srsResult = core::srs::left(field, piece, fromRotate, toBlocks, fromX, fromY);
                    if (0 <= srsResult && srsResult % 5 == 4) {
                        // T-Spin Regular
                        int baseAttack = numCleared * 2;
                        return b2b ? baseAttack + 1 : baseAttack;
                    }

                    // Mini or No T-Spin
                }
            }
        }

        return 0;
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
        if (isWorseThanBest(configure.leastLineClears, best, candidate))  {
            return;
        }

        auto depth = candidate.depth;

        auto &pieces = configure.pieces;
        auto &moves = configure.movePool[depth];

        auto currentIndex = candidate.currentIndex;
        assert(0 <= currentIndex && currentIndex <= configure.pieceSize);
        auto holdIndex = candidate.holdIndex;
        assert(-1 <= holdIndex && holdIndex < static_cast<long long int>(configure.pieceSize));

        auto holdCount = candidate.holdCount;

        bool canUseCurrent = currentIndex < configure.pieceSize;
        if (canUseCurrent) {
            assert(static_cast<unsigned int>(currentIndex) < pieces.size());
            auto &current = pieces[currentIndex];

            moves.clear();
            move(configure, candidate, solution, moves, current, currentIndex + 1, holdIndex, holdCount);
        }

        if (0 <= holdIndex) {
            assert(static_cast<unsigned int>(holdIndex) < pieces.size());

            // Hold exists
            if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                auto &hold = pieces[holdIndex];

                moves.clear();
                move(configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex, holdCount + 1);
            }
        } else {
            assert(canUseCurrent);

            // Empty hold
            auto nextIndex = currentIndex + 1;
            assert(static_cast<unsigned int>(nextIndex) < pieces.size() + 1);

            if (nextIndex < configure.pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                assert(static_cast<unsigned int>(nextIndex) < pieces.size());
                auto &next = pieces[nextIndex];

                moves.clear();
                move(configure, candidate, solution, moves, next, nextIndex + 1, currentIndex, holdCount + 1);
            }
        }
    }

    template<>
    void PerfectFinder<core::srs::MoveGenerator>::accept(const Configure &configure, const Record &record) {
        assert(!best.solution.empty());

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

        auto currentTSpinAttack = candidate.tSpinAttack;
        auto currentB2b = candidate.b2b;

        auto nextLeftNumOfT = pieceType == core::PieceType::T ? candidate.leftNumOfT - 1 : candidate.leftNumOfT;

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

            int tSpinAttack = getAttackIfTSpin(reachable, factory, field, pieceType, move, numCleared, currentB2b);

            int nextSoftdropCount = move.harddrop ? softdropCount : softdropCount + 1;
            int nextLineClearCount = 0 < numCleared ? lineClearCount + 1 : lineClearCount;
            int nextCurrentCombo = 0 < numCleared ? currentCombo + 1 : 0;
            int nextMaxCombo = maxCombo < nextCurrentCombo ? nextCurrentCombo : maxCombo;
            int nextTSpinAttack = currentTSpinAttack + tSpinAttack;
            bool nextB2b = 0 < numCleared ? (tSpinAttack != 0 || numCleared == 4) : currentB2b;

            int nextLeftLine = leftLine - numCleared;
            if (nextLeftLine == 0) {
                auto record = Record{
                        solution, nextSoftdropCount, nextHoldCount, nextLineClearCount, nextMaxCombo, nextTSpinAttack
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
                    nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                    nextTSpinAttack, nextB2b, nextLeftNumOfT,
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

        // Count up T
        int leftNumOfT = static_cast<int>(std::count(pieces.begin(), pieces.end(), core::PieceType::T));

        // Create candidate
        Candidate candidate = holdEmpty
                              ? Candidate{freeze, 0, -1, maxLine, 0, 0, 0, 0, initCombo, initCombo, 0, true, leftNumOfT}
                              : Candidate{freeze, 1, 0, maxLine, 0, 0, 0, 0, initCombo, initCombo, 0, true, leftNumOfT};

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
        return run(field, pieces, maxDepth, maxLine, holdEmpty, true, 0);
    }
}
