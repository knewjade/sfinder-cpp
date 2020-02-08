#ifndef FINDER_PERFECT_CLEAR_ASYNC_HPP
#define FINDER_PERFECT_CLEAR_ASYNC_HPP

#include "types.hpp"
#include "perfect_clear.hpp"
#include "thread_pool.hpp"

#include "../core/moves.hpp"

namespace finder {
    // Entry point to find best perfect clear
    template<class M = core::srs::MoveGenerator>
    class ConcurrentPerfectClearFinder {
    public:
        ConcurrentPerfectClearFinder<M>(const core::Factory &factory, ThreadPool &threadPool)
                : factory_(factory), threadPool_(threadPool) {
        }

        // If `alwaysRegularAttack` is true, mini spin is judged as regular attack
        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces,
                int maxDepth, int maxLine, bool holdEmpty, bool leastLineClears, int initCombo, bool initB2b,
                SearchTypes searchTypes, bool alwaysRegularAttack, uint8_t lastHoldPriority
        ) {
            assert(1 <= maxDepth);

            // Copy field
            auto freeze = core::Field(field);

            // original
            // Initialize configure
            auto movePool = std::vector<std::vector<core::Move>>{};
            const auto originalConfigure = Configure{
                    pieces,
                    movePool,
                    maxDepth,
                    static_cast<int>(pieces.size()),
                    leastLineClears,
                    alwaysRegularAttack,
                    lastHoldPriority,
            };

            // Reset status
            runnerStatus_.resume();

            switch (searchTypes) {
                case SearchTypes::Fast: {
                    using Candidate = FastCandidate;
                    using Record = FastRecord;

                    // Create candidate
                    auto candidate = holdEmpty
                                     ? Candidate{0, -1, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo}
                                     : Candidate{1, 0, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo};

                    // premove
                    auto moves = std::vector<core::Move>();
                    auto firstCandidates = std::vector<PreOperation<Candidate>>();
                    premove(alwaysRegularAttack, maxDepth, freeze, candidate, moves, pieces, firstCandidates);

                    // Find solution by concurrent
                    Recorder<Candidate, Record> recorder{};
                    std::mutex mutex;

                    auto futures = std::vector<std::future<bool>>(firstCandidates.size());

                    for (int index = 0; index < futures.size(); ++index) {
                        auto &firstCandidate = firstCandidates[index];
                        Callable<bool> callable = [&](const TaskStatus &taskStatus) {
                            if (taskStatus.notWorking()) {
                                return false;
                            }

                            auto nextField = calcNextField(freeze, firstCandidate);

                            auto configure = createConfigure(pieces, maxDepth, leastLineClears, alwaysRegularAttack,
                                                             lastHoldPriority);

                            auto moveGenerator = M(factory_);
                            auto reachable = core::srs_rotate_end::Reachable(factory_);
                            auto finder = PCFindRunner<M, Candidate, Record>(factory_, moveGenerator, reachable, runnerStatus_);

                            auto record = finder.runRecord(configure, nextField, firstCandidate.candidate);

                            if (record.solution.empty()) {
                                return false;
                            }

                            record.solution[0].pieceType = firstCandidate.pieceType;
                            record.solution[0].rotateType = firstCandidate.rotateType;
                            record.solution[0].x = firstCandidate.x;
                            record.solution[0].y = firstCandidate.y;

                            auto newRecord = Candidate{
                                    record.currentIndex,
                                    record.holdIndex, record.leftLine,
                                    record.depth, record.softdropCount,
                                    record.holdCount, record.lineClearCount,
                                    record.currentCombo, record.maxCombo,
                            };

                            {
                                std::lock_guard<std::mutex> guard(mutex);
                                if (recorder.shouldUpdate(originalConfigure, newRecord)) {
                                    recorder.update(originalConfigure, newRecord, record.solution);
                                }
                            }

                            return true;
                        };
                        futures[index] = threadPool_.execute(callable);
                    }

                    // Wait
                    for (auto &future : futures) {
                        future.get();
                    }

                    // Return solution
                    auto best = recorder.best();
                    return best.solution.empty() ? kNoSolution : std::vector<Operation>(best.solution);
                }
                case SearchTypes::TSpin: {
                    assert(!alwaysRegularAttack);  // Support no mini only

                    using Candidate = TSpinCandidate;
                    using Record = TSpinRecord;

                    // Count up T
                    int leftNumOfT = std::count(pieces.begin(), pieces.end(), core::PieceType::T);

                    // Create candidate
                    auto candidate = holdEmpty
                                     ? Candidate{0, -1, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo, 0, initB2b, leftNumOfT}
                                     : Candidate{1, 0, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo, 0, initB2b, leftNumOfT};

                    // premove
                    auto moves = std::vector<core::Move>();
                    auto firstCandidates = std::vector<PreOperation<Candidate>>();
                    premove(alwaysRegularAttack, maxDepth, freeze, candidate, moves, pieces, firstCandidates);

                    // Find solution by concurrent
                    Recorder<Candidate, Record> recorder{};
                    std::mutex mutex;

                    auto futures = std::vector<std::future<bool>>(firstCandidates.size());

                    for (int index = 0; index < futures.size(); ++index) {
                        auto &firstCandidate = firstCandidates[index];
                        Callable<bool> callable = [&](const TaskStatus &taskStatus) {
                            if (taskStatus.notWorking()) {
                                return false;
                            }

                            auto nextField = calcNextField(freeze, firstCandidate);

                            auto configure = createConfigure(pieces, maxDepth, leastLineClears, alwaysRegularAttack,
                                                             lastHoldPriority);

                            auto moveGenerator = M(factory_);
                            auto reachable = core::srs_rotate_end::Reachable(factory_);
                            auto finder = PCFindRunner<M, Candidate, Record>(factory_, moveGenerator, reachable, runnerStatus_);
                            auto record = finder.runRecord(configure, nextField, firstCandidate.candidate);

                            if (record.solution.empty()) {
                                return false;
                            }

                            record.solution[0].pieceType = firstCandidate.pieceType;
                            record.solution[0].rotateType = firstCandidate.rotateType;
                            record.solution[0].x = firstCandidate.x;
                            record.solution[0].y = firstCandidate.y;

                            auto newRecord = Candidate{
                                    record.currentIndex,
                                    record.holdIndex, record.leftLine,
                                    record.depth, record.softdropCount,
                                    record.holdCount, record.lineClearCount,
                                    record.currentCombo, record.maxCombo,
                                    record.tSpinAttack, record.b2b,
                                    record.leftNumOfT,
                            };

                            {
                                std::lock_guard<std::mutex> guard(mutex);
                                if (recorder.shouldUpdate(originalConfigure, newRecord)) {
                                    recorder.update(originalConfigure, newRecord, record.solution);
                                }
                            }

                            return true;
                        };
                        futures[index] = threadPool_.execute(callable);
                    }

                    // Wait
                    for (auto &future : futures) {
                        future.get();
                    }

                    // Return solution
                    auto best = recorder.best();
                    return best.solution.empty() ? kNoSolution : std::vector<Operation>(best.solution);
                }
                case SearchTypes::AllSpins: {
                    using Candidate = AllSpinsCandidate;
                    using Record = AllSpinsRecord;

                    // Create candidate
                    auto candidate = holdEmpty
                                     ? Candidate{0, -1, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo, 0, initB2b}
                                     : Candidate{1, 0, maxLine, 0, 0, 0, 0,
                                                 initCombo, initCombo, 0, initB2b};

                    // premove
                    auto moves = std::vector<core::Move>();
                    auto firstCandidates = std::vector<PreOperation<Candidate>>();
                    premove(alwaysRegularAttack, maxDepth, freeze, candidate, moves, pieces, firstCandidates);

                    // Find solution by concurrent
                    Recorder<Candidate, Record> recorder{};
                    std::mutex mutex;

                    auto futures = std::vector<std::future<bool>>(firstCandidates.size());

                    for (int index = 0; index < futures.size(); ++index) {
                        auto &firstCandidate = firstCandidates[index];
                        Callable<bool> callable = [&](const TaskStatus &taskStatus) {
                            if (taskStatus.notWorking()) {
                                return false;
                            }

                            auto nextField = calcNextField(freeze, firstCandidate);

                            auto configure = createConfigure(pieces, maxDepth, leastLineClears, alwaysRegularAttack,
                                                             lastHoldPriority);

                            auto moveGenerator = M(factory_);
                            auto reachable = core::srs_rotate_end::Reachable(factory_);
                            auto finder = PCFindRunner<M, Candidate, Record>(factory_, moveGenerator, reachable, runnerStatus_);
                            auto record = finder.runRecord(configure, nextField, firstCandidate.candidate);

                            if (record.solution.empty()) {
                                return false;
                            }

                            record.solution[0].pieceType = firstCandidate.pieceType;
                            record.solution[0].rotateType = firstCandidate.rotateType;
                            record.solution[0].x = firstCandidate.x;
                            record.solution[0].y = firstCandidate.y;

                            auto newRecord = Candidate{
                                    record.currentIndex,
                                    record.holdIndex, record.leftLine,
                                    record.depth, record.softdropCount,
                                    record.holdCount, record.lineClearCount,
                                    record.currentCombo, record.maxCombo,
                                    record.spinAttack, record.b2b,
                            };

                            {
                                std::lock_guard<std::mutex> guard(mutex);
                                if (recorder.shouldUpdate(originalConfigure, newRecord)) {
                                    recorder.update(originalConfigure, newRecord, record.solution);
                                }
                            }

                            return true;
                        };
                        futures[index] = threadPool_.execute(callable);
                    }

                    // Wait
                    for (auto &future : futures) {
                        future.get();
                    }

                    // Return solution
                    auto best = recorder.best();
                    return best.solution.empty() ? kNoSolution : std::vector<Operation>(best.solution);
                }
                default: {
                    assert(false);
                    throw std::runtime_error("Illegal search types: value=" + std::to_string(searchTypes));
                }
            }
        }

        template<class Candidate>
        core::Field calcNextField(
                const core::Field &field, const PreOperation<Candidate> &firstCandidate
        ) const {
            auto freeze = core::Field(field);
            auto &blocks = factory_.get(firstCandidate.pieceType, firstCandidate.rotateType);
            freeze.put(blocks, firstCandidate.x, firstCandidate.y);
            freeze.clearLine();
            return freeze;
        }

        // searchType refers to code
        Solution run(
                const core::Field &field, const std::vector<core::PieceType> &pieces, int maxDepth,
                int maxLine, bool holdEmpty, int searchType, bool leastLineClears,
                int initCombo, bool initB2b, bool twoLineFollowUp
        ) {
            // Check last hold that can take 2 PC
            uint8_t lastHoldPriority = 0U;
            if (maxDepth + 5 <= pieces.size() && twoLineFollowUp) {
                std::vector<core::PieceType> nextPieces(pieces.cbegin() + maxDepth, pieces.cend());
                if (holdEmpty && canTake2LinePC(nextPieces)) {
                    lastHoldPriority |= 0b10000000U;
                }

                for (unsigned int pieceType = 0; pieceType < 7; ++pieceType) {
                    nextPieces[0] = static_cast<core::PieceType>(pieceType);
                    if (canTake2LinePC(nextPieces)) {
                        lastHoldPriority |= 1U << pieceType;
                    }
                }
            }

            if (lastHoldPriority == 0U) {
                lastHoldPriority = 0b11111111U;
            }

            // Decide parameters
            switch (searchType) {
                case 0: {
                    // No softdrop is top priority
                    return run(
                            field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, initB2b, SearchTypes::Fast,
                            false, lastHoldPriority
                    );
                }
                case 1: {
                    // T-Spin is top priority (mini is zero attack)
                    return run(
                            field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, initB2b, SearchTypes::TSpin,
                            false, lastHoldPriority
                    );
                }
                case 2: {
                    // All-Spins is top priority (all spins are judged as regular attack)
                    return run(
                            field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, initB2b,
                            SearchTypes::AllSpins,
                            true, lastHoldPriority
                    );
                }
                case 3: {
                    // All-Spins is top priority (mini is zero attack)
                    return run(
                            field, pieces, maxDepth, maxLine, holdEmpty, true, initCombo, initB2b,
                            SearchTypes::AllSpins,
                            false, lastHoldPriority
                    );
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
            return run(
                    field, pieces, maxDepth, maxLine, holdEmpty, SearchTypes::TSpin, true, 0, true, false
            );
        }

        void abort() {
            runnerStatus_.abort();
            threadPool_.abort();
        }

    private:
        template<class C>
        Mover<M, C> createMover() const {
            thread_local auto moveGenerator = M(factory_);
            thread_local auto reachable = core::srs_rotate_end::Reachable(factory_);
            auto mover = Mover<M, C>(factory_, moveGenerator, reachable);
            return mover;
        }

        Configure createConfigure(
                const std::vector<core::PieceType> &pieces,
                int maxDepth, bool leastLineClears, bool alwaysRegularAttack, uint8_t lastHoldPriority
        ) const {
            // Initialize moves
            thread_local auto movePool = std::vector<std::vector<core::Move>>{};
            movePool.clear();
            for (int index = 0; index < maxDepth; ++index) {
                movePool.emplace_back();
            }

            // Initialize configure
            const auto configure = Configure{
                    pieces,
                    movePool,
                    maxDepth,
                    static_cast<int>(pieces.size()),
                    leastLineClears,
                    alwaysRegularAttack,
                    lastHoldPriority,
            };

            return configure;
        }

        template<class C>
        void premove(
                bool alwaysRegularAttack,
                int maxDepth,
                const core::Field &field,
                const C &candidate,
                std::vector<core::Move> &moves,
                const std::vector<core::PieceType> &pieces,
                std::vector<PreOperation<C>> &output
        ) const {
            auto mover = createMover<C>();

            int pieceSize = pieces.size();

            auto currentIndex = candidate.currentIndex;
            assert(0 <= currentIndex && currentIndex <= pieceSize);
            auto holdIndex = candidate.holdIndex;
            assert(-1 <= holdIndex && holdIndex < pieceSize);

            auto holdCount = candidate.holdCount;

            bool canUseCurrent = currentIndex < pieceSize;
            if (canUseCurrent) {
                assert(currentIndex < pieces.size());
                auto &current = pieces[currentIndex];

                moves.clear();
                mover.premove(
                        alwaysRegularAttack, maxDepth, field, candidate,
                        moves, current, currentIndex + 1, holdIndex, holdCount, output
                );
            }

            if (0 <= holdIndex) {
                assert(holdIndex < pieces.size());

                // Hold exists
                if (!canUseCurrent || pieces[currentIndex] != pieces[holdIndex]) {
                    auto &hold = pieces[holdIndex];

                    moves.clear();
                    mover.premove(
                            alwaysRegularAttack, maxDepth, field, candidate,
                            moves, hold, currentIndex + 1, currentIndex, holdCount + 1, output
                    );
                }
            } else {
                assert(canUseCurrent);

                // Empty hold
                int nextIndex = currentIndex + 1;
                assert(nextIndex < pieces.size() + 1);

                if (nextIndex < pieceSize && pieces[currentIndex] != pieces[nextIndex]) {
                    assert(nextIndex < pieces.size());
                    auto &next = pieces[nextIndex];

                    moves.clear();
                    mover.premove(
                            alwaysRegularAttack, maxDepth, field, candidate,
                            moves, next, nextIndex + 1, currentIndex, holdCount + 1, output
                    );
                }
            }
        }

        const core::Factory &factory_;
        ThreadPool &threadPool_;
        RunnerStatus runnerStatus_{};
    };
}

#endif //FINDER_PERFECT_CLEAR_ASYNC_HPP