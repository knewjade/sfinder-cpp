#ifndef FINDER_PERFECT_HPP
#define FINDER_PERFECT_HPP

#include "types.hpp"
#include "spins.hpp"

#include "../core/piece.hpp"
#include "../core/moves.hpp"
#include "../core/types.hpp"

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

    enum SearchTypes {
        Fast,
        TSpin,
        AllSpins,
    };

    template<class M, class C>
    class Mover;

    template<class C, class R>
    class Recorder;

    // Main finder implementation
    template<class M = core::srs::MoveGenerator, class C = TSpinCandidate, class R = TSpinRecord>
    class PCFindRunner {
    public:
        PCFindRunner<M, C, R>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : mover(Mover<M, C>(factory, moveGenerator, reachable)), recorder(Recorder<C, R>()) {
        }

        Solution run(const Configure &configure, const C &candidate) {
            recorder.clear();

            // Initialize solution
            Solution solution(configure.maxDepth);
            std::fill(solution.begin(), solution.end(), Operation{
                    core::PieceType::T, core::RotateType::Spawn, -1, -1
            });

            // Execute
            search(configure, candidate, solution);

            auto &best = recorder.best();
            return best.solution.empty() ? kNoSolution : std::vector<Operation>(best.solution);
        }

        void search(const Configure &configure, const C &candidate, Solution &solution) {
            if (recorder.isWorseThanBest(configure.leastLineClears, candidate)) {
                return;
            }

            auto depth = candidate.depth;

            auto &pieces = configure.pieces;
            auto &moves = configure.movePool[depth];

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
                mover.move(
                        configure, candidate, solution, moves, current, currentIndex + 1, holdIndex, holdCount, this
                );
            }

            if (0 <= holdIndex) {
                assert(holdIndex < pieces.size());

                // Hold exists
                if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                    auto &hold = pieces[holdIndex];

                    moves.clear();
                    mover.move(
                            configure, candidate, solution, moves, hold, currentIndex + 1, currentIndex,
                            holdCount + 1, this
                    );
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
                    mover.move(
                            configure, candidate, solution, moves, next, nextIndex + 1, currentIndex,
                            holdCount + 1, this
                    );
                }
            }
        }

        void accept(const Configure &configure, const Solution &solution, const C &current) {
            if (recorder.shouldUpdate(configure.leastLineClears, current)) {
                recorder.update(solution, current);
            }
        }

    private:
        Mover<M, C> mover;
        Recorder<C, R> recorder;
    };

    // Mover implementations
    template<class M>
    class Mover<M, TSpinCandidate> {
    public:
        Mover<M, TSpinCandidate>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : factory(factory), moveGenerator(moveGenerator), reachable(reachable) {
        }

        void move(
                const Configure &configure,
                const TSpinCandidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount,
                PCFindRunner<M, TSpinCandidate, TSpinRecord> *finder
        ) {
            assert(0 < candidate.leftLine);

            auto nextLeftNumOfT = pieceType == core::PieceType::T ? candidate.leftNumOfT - 1 : candidate.leftNumOfT;

            moveGenerator.search(moves, candidate.field, pieceType, candidate.leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(pieceType, move.rotateType);

                auto freeze = core::Field(candidate.field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                auto &operation = solution[candidate.depth];
                operation.pieceType = pieceType;
                operation.rotateType = move.rotateType;
                operation.x = move.x;
                operation.y = move.y;

                int tSpinAttack = getAttackIfTSpin(
                        reachable, factory, candidate.field, pieceType, move, numCleared, candidate.b2b
                );

                int nextSoftdropCount = move.harddrop ? candidate.softdropCount : candidate.softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? candidate.lineClearCount + 1 : candidate.lineClearCount;
                int nextCurrentCombo = 0 < numCleared ? candidate.currentCombo + 1 : 0;
                int nextMaxCombo = candidate.maxCombo < nextCurrentCombo ? nextCurrentCombo : candidate.maxCombo;
                int nextTSpinAttack = candidate.tSpinAttack + tSpinAttack;
                bool nextB2b = 0 < numCleared ? (tSpinAttack != 0 || numCleared == 4) : candidate.b2b;

                auto nextDepth = candidate.depth + 1;

                int nextLeftLine = candidate.leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto bestCandidate = TSpinCandidate{
                            freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                            nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                            nextTSpinAttack, nextB2b, nextLeftNumOfT,
                    };
                    finder->accept(configure, solution, bestCandidate);
                    return;
                }

                if (configure.maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                auto nextCandidate = TSpinCandidate{
                        freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                        nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                        nextTSpinAttack, nextB2b, nextLeftNumOfT,
                };
                finder->search(configure, nextCandidate, solution);
            }
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };

    template<class M>
    class Mover<M, FastCandidate> {
    public:
        Mover<M, FastCandidate>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : factory(factory), moveGenerator(moveGenerator), reachable(reachable) {
        }

        void move(
                const Configure &configure,
                const FastCandidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount,
                PCFindRunner<M, FastCandidate, FastRecord> *finder
        ) {
            assert(0 < candidate.leftLine);

            moveGenerator.search(moves, candidate.field, pieceType, candidate.leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(pieceType, move.rotateType);

                auto freeze = core::Field(candidate.field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                auto &operation = solution[candidate.depth];
                operation.pieceType = pieceType;
                operation.rotateType = move.rotateType;
                operation.x = move.x;
                operation.y = move.y;

                int nextSoftdropCount = move.harddrop ? candidate.softdropCount : candidate.softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? candidate.lineClearCount + 1 : candidate.lineClearCount;

                auto nextDepth = candidate.depth + 1;

                int nextLeftLine = candidate.leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto bestCandidate = FastCandidate{
                            freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                            nextSoftdropCount, nextHoldCount, nextLineClearCount
                    };
                    finder->accept(configure, solution, bestCandidate);
                    return;
                }

                if (configure.maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                auto nextCandidate = FastCandidate{
                        freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                        nextSoftdropCount, nextHoldCount, nextLineClearCount
                };
                finder->search(configure, nextCandidate, solution);
            }
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };

    template<class M>
    class Mover<M, AllSpinsCandidate> {
    public:
        Mover<M, AllSpinsCandidate>(
                const core::Factory &factory, M &moveGenerator, core::srs_rotate_end::Reachable &reachable
        ) : factory(factory), moveGenerator(moveGenerator), reachable(reachable) {
        }

        void move(
                const Configure &configure,
                const AllSpinsCandidate &candidate,
                Solution &solution,
                std::vector<core::Move> &moves,
                core::PieceType pieceType,
                int nextIndex,
                int nextHoldIndex,
                int nextHoldCount,
                PCFindRunner<M, AllSpinsCandidate, AllSpinsRecord> *finder
        ) {
            assert(0 < candidate.leftLine);

            auto getAttack = configure.alwaysRegularAttack ? getAttackIfAllSpins<true> : getAttackIfAllSpins<false>;

            moveGenerator.search(moves, candidate.field, pieceType, candidate.leftLine);

            for (const auto &move : moves) {
                auto &blocks = factory.get(pieceType, move.rotateType);

                auto freeze = core::Field(candidate.field);
                freeze.put(blocks, move.x, move.y);

                int numCleared = freeze.clearLineReturnNum();

                auto &operation = solution[candidate.depth];
                operation.pieceType = pieceType;
                operation.rotateType = move.rotateType;
                operation.x = move.x;
                operation.y = move.y;

                int spinAttack = getAttack(
                        factory, candidate.field, pieceType, move, numCleared, candidate.b2b
                );

                int nextSoftdropCount = move.harddrop ? candidate.softdropCount : candidate.softdropCount + 1;
                int nextLineClearCount = 0 < numCleared ? candidate.lineClearCount + 1 : candidate.lineClearCount;
                int nextCurrentCombo = 0 < numCleared ? candidate.currentCombo + 1 : 0;
                int nextMaxCombo = candidate.maxCombo < nextCurrentCombo ? nextCurrentCombo : candidate.maxCombo;
                int nextTSpinAttack = candidate.spinAttack + spinAttack;
                bool nextB2b = 0 < numCleared ? (spinAttack != 0 || numCleared == 4) : candidate.b2b;

                auto nextDepth = candidate.depth + 1;

                int nextLeftLine = candidate.leftLine - numCleared;
                if (nextLeftLine == 0) {
                    auto bestCandidate = AllSpinsCandidate{
                            freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                            nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                            nextTSpinAttack, nextB2b,
                    };
                    finder->accept(configure, solution, bestCandidate);
                    return;
                }

                if (configure.maxDepth <= nextDepth) {
                    continue;
                }

                if (!validate(freeze, nextLeftLine)) {
                    continue;
                }

                auto nextCandidate = AllSpinsCandidate{
                        freeze, nextIndex, nextHoldIndex, nextLeftLine, nextDepth,
                        nextSoftdropCount, nextHoldCount, nextLineClearCount, nextCurrentCombo, nextMaxCombo,
                        nextTSpinAttack, nextB2b,
                };
                finder->search(configure, nextCandidate, solution);
            }
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };

    // Recorder defines
    template<>
    class Recorder<TSpinCandidate, TSpinRecord> {
    public:
        void clear();

        void update(const Solution &solution, const TSpinCandidate &current);

        [[nodiscard]] bool isWorseThanBest(bool leastLineClears, const TSpinCandidate &current) const;

        [[nodiscard]] bool shouldUpdate(bool leastLineClears, const TSpinCandidate &newRecord) const;

        [[nodiscard]] const TSpinRecord &best() const {
            return best_;
        }

    private:
        TSpinRecord best_;
    };

    template<>
    class Recorder<FastCandidate, FastRecord> {
    public:
        void clear();

        void update(const Solution &solution, const FastCandidate &current);

        [[nodiscard]] bool isWorseThanBest(bool leastLineClears, const FastCandidate &current) const;

        [[nodiscard]] bool shouldUpdate(bool leastLineClears, const FastCandidate &newRecord) const;

        [[nodiscard]] const FastRecord &best() const {
            return best_;
        }

    private:
        FastRecord best_;
    };

    template<>
    class Recorder<AllSpinsCandidate, AllSpinsRecord> {
    public:
        void clear();

        void update(const Solution &solution, const AllSpinsCandidate &current);

        [[nodiscard]] bool isWorseThanBest(bool leastLineClears, const AllSpinsCandidate &current) const;

        [[nodiscard]] bool shouldUpdate(bool leastLineClears, const AllSpinsCandidate &newRecord) const;

        [[nodiscard]] const AllSpinsRecord &best() const {
            return best_;
        }

    private:
        AllSpinsRecord best_;
    };

    // Entry point to find best perfect clear
    template<class M = core::srs::MoveGenerator>
    class PerfectClearFinder {
    public:
        PerfectClearFinder<M>(const core::Factory &factory, M &moveGenerator)
                : factory(factory), moveGenerator(moveGenerator), reachable(core::srs_rotate_end::Reachable(factory)) {
        }

        // If `alwaysRegularAttack` is true, mini spin is judged as regular attack
        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty, bool leastLineClears, int initCombo,
                SearchTypes searchTypes, bool alwaysRegularAttack
        ) {
            assert(1 <= maxDepth);

            // Copy field
            auto freeze = core::Field(field);

            // Initialize moves
            std::vector<std::vector<core::Move>> movePool(maxDepth);
            for (int index = 0; index < maxDepth; ++index) {
                movePool[index] = std::vector<core::Move>{};
            }

            // Initialize configure
            const auto configure = Configure{
                    pieces,
                    movePool,
                    maxDepth,
                    static_cast<int>(pieces.size()),
                    leastLineClears,
                    alwaysRegularAttack,
            };

            switch (searchTypes) {
                case SearchTypes::Fast: {
                    // Create candidate
                    auto candidate = holdEmpty
                                     ? FastCandidate{freeze, 0, -1, maxLine, 0, 0, 0, 0,
                                                     initCombo, initCombo}
                                     : FastCandidate{freeze, 1, 0, maxLine, 0, 0, 0, 0,
                                                     initCombo, initCombo};

                    auto finder = PCFindRunner<M, FastCandidate, FastRecord>(factory, moveGenerator, reachable);
                    return finder.run(configure, candidate);
                }
                case SearchTypes::TSpin: {
                    assert(!alwaysRegularAttack);  // Support no mini only

                    // Count up T
                    int leftNumOfT = std::count(pieces.begin(), pieces.end(), core::PieceType::T);

                    // Create candidate
                    auto candidate = holdEmpty
                                     ? TSpinCandidate{freeze, 0, -1, maxLine, 0, 0, 0, 0,
                                                      initCombo, initCombo, 0, true, leftNumOfT}
                                     : TSpinCandidate{freeze, 1, 0, maxLine, 0, 0, 0, 0,
                                                      initCombo, initCombo, 0, true, leftNumOfT};

                    auto finder = PCFindRunner<M, TSpinCandidate, TSpinRecord>(factory, moveGenerator, reachable);
                    return finder.run(configure, candidate);
                }
                case SearchTypes::AllSpins: {
                    // Create candidate
                    auto candidate = holdEmpty
                                     ? AllSpinsCandidate{freeze, 0, -1, maxLine, 0, 0, 0, 0,
                                                         initCombo, initCombo, 0, true}
                                     : AllSpinsCandidate{freeze, 1, 0, maxLine, 0, 0, 0, 0,
                                                         initCombo, initCombo, 0, true};

                    auto finder = PCFindRunner<M, AllSpinsCandidate, AllSpinsRecord>(factory, moveGenerator, reachable);
                    return finder.run(configure, candidate);
                }
                default: {
                    assert(false);
                    throw std::runtime_error("Illegal search types: value=" + std::to_string(searchTypes));
                }
            }
        }

        // searchType refers to code
        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces, int maxDepth,
                int maxLine, bool holdEmpty, int searchType, bool leastLineClears, int initCombo
        ) {
            switch (searchType) {
                case 0: {
                    // No softdrop is top priority
                    return run(field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, SearchTypes::Fast, false);
                }
                case 1: {
                    // T-Spin is top priority (mini is zero attack)
                    return run(field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, SearchTypes::TSpin, false);
                }
                case 2: {
                    // All-Spins is top priority (all spins are judged as regular attack)
                    return run(field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, SearchTypes::AllSpins, true);
                }
                case 3: {
                    // All-Spins is top priority (mini is zero attack)
                    return run(field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, SearchTypes::AllSpins, false);
                }
                default: {
                    throw std::runtime_error("Illegal search type: value=" + std::to_string(searchType));
                }
            }
        }

        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty
        ) {
            return run(field, pieces, maxDepth, maxLine, holdEmpty, true, 0, SearchTypes::TSpin, false);
        }

    private:
        const core::Factory &factory;
        M &moveGenerator;
        core::srs_rotate_end::Reachable reachable;
    };
}

#endif //FINDER_PERFECT_HPP
