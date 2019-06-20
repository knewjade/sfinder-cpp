#include "ren.hpp"

namespace finder::ren {
    namespace {
        bool shouldUpdate(
                const Record &oldRecord, const Record &newRecord
        ) {
            if (newRecord.maxCombo != oldRecord.maxCombo) {
                return oldRecord.maxCombo < newRecord.maxCombo;
            }

            if (newRecord.softdropCount != oldRecord.softdropCount) {
                return newRecord.softdropCount < oldRecord.softdropCount;
            }

            return newRecord.holdCount < oldRecord.holdCount;
        }
    }

    template<>
    void Finder<core::srs::MoveGenerator>::move(
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
    void Finder<core::srs::MoveGenerator>::search(
            const Configure &configure,
            const Candidate &candidate,
            Solution &solution
    ) {
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
    void Finder<core::srs::MoveGenerator>::accept(const Configure &configure, const Record &record) {
        assert(!best.solution.empty());

        if (best.solution[0].x == -1 || shouldUpdate(best, record)) {
            best = Record(record);
        }
    }

    template<>
    void Finder<core::srs::MoveGenerator>::move(
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
        auto &field = candidate.field;

        auto holdCount = candidate.holdCount;
        auto softdropCount = candidate.softdropCount;

        auto currentCombo = candidate.currentCombo;

        moveGenerator.search(moves, field, pieceType, core::kMaxFieldHeight);

        for (const auto &move : moves) {
            auto &blocks = factory.get(pieceType, move.rotateType);

            auto freeze = core::Field(field);
            freeze.put(blocks, move.x, move.y);

            int numCleared = freeze.clearLineReturnNum();

            if (numCleared == 0) {
                if (best.maxCombo <= currentCombo) {
                    auto record = Record{
                            solution, softdropCount, holdCount, currentCombo
                    };
                    accept(configure, record);
                }
                continue;
            }

            solution[depth].pieceType = pieceType;
            solution[depth].rotateType = move.rotateType;
            solution[depth].x = move.x;
            solution[depth].y = move.y;

            int nextSoftdropCount = move.harddrop ? softdropCount : softdropCount + 1;

            auto nextCurrentCombo = currentCombo + 1;
            auto nextDepth = depth + 1;
            if (configure.pieceSize <= nextDepth) {
                auto record = Record{
                        solution, nextSoftdropCount, nextHoldCount, nextCurrentCombo
                };
                accept(configure, record);
                continue;
            }

            auto nextCandidate = Candidate{
                    freeze, nextIndex, nextHoldIndex, nextDepth, nextSoftdropCount, nextHoldCount, nextCurrentCombo
            };
            search(configure, nextCandidate, solution);
        }
    }

    template<>
    int Finder<core::srs::MoveGenerator>::run(
            const core::Field &field, const std::vector<core::PieceType> &pieces, bool holdEmpty
    ) {
        int maxDepth = pieces.size();

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
                static_cast<int>(pieces.size()),
        };

        // Create candidate
        Candidate candidate = holdEmpty
                              ? Candidate{freeze, 0, -1, 0, 0, 0, 0}
                              : Candidate{freeze, 1, 0, 0, 0, 0, 0};

        // Create current record & best record
        best = Record{
                std::vector(solution),
                INT_MAX,
                INT_MAX,
                -1,
        };

        // Execute
        search(configure, candidate, solution);

//        return best.solution[0].x == -1 ? kNoSolution : std::vector<Operation>(best.solution);
        return best.maxCombo;
    }
}
