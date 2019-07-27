#include "gtest/gtest.h"

#include <chrono>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

#include "core/field.hpp"
#include "core/moves.hpp"
#include "core/pieces.hpp"

#include "sfinder/permutations.hpp"
#include "sfinder/lookup.hpp"
#include "sfinder/spins.hpp"
#include "sfinder/perfect_clear/full_finder.hpp"
#include "sfinder/percentage.hpp"

namespace finder {
    namespace {
        struct Configure {
            const std::vector<core::PieceType> &pieces;
            std::vector<std::vector<core::Move>> &movePools;
            std::set<core::Bitboard> &fieldSet;
            const int maxDepth;
            const int pieceSize;
        };

        namespace {
            enum From {
                None = 0,
                Right = 1,
                Left = 2,
            };

            inline void putAndMove(core::Field &field, int x, int y, From from) {
                // 壁なら終了
                if (!field.isEmpty(x, y))
                    return;

                // 自分自身を塗りつぶす
                field.setBlock(x, y);

                // 移動する
                if (0 <= y - 1) {
                    putAndMove(field, x, y - 1, From::None);
                }

                if (from != From::Right && x + 1 < core::kFieldWidth) {
                    putAndMove(field, x + 1, y, From::Left);
                }

                if (from != From::Left && 0 <= x - 1) {
                    putAndMove(field, x - 1, y, From::Right);
                }
            }

            inline bool validateHole(const core::Field &field, int maxLine) {
                auto freeze = core::Field(field);
                for (int x = 0; x < core::kFieldWidth; x++)
                    putAndMove(freeze, x, maxLine - 1, From::None);
                return freeze.numOfAllBlocks() == maxLine * 10;
            }


            inline bool validate(const core::Field &field, int maxLine) {
                int sum = maxLine - field.getBlockOnX(0, maxLine);
                for (int x = 1; x < core::kFieldWidth; x++) {
                    int emptyCountInColumn = maxLine - field.getBlockOnX(x, maxLine);
                    if (field.isWallBetween(x, maxLine)) {
                        if (sum % 4 != 0)
                            return false;
                        sum = emptyCountInColumn;
                    } else {
                        sum += emptyCountInColumn;
                    }
                }

                if (sum % 4 != 0) {
                    return false;
                }

                return validateHole(field, maxLine);
            }
        }

        template<class T = core::harddrop::MoveGenerator>
        class MoveFinder {
        public:
            MoveFinder<T>(const core::Factory &factory, T &moveGenerator) :
                    factory_(factory), moveGenerator_(moveGenerator),
                    reachable_(core::srs_rotate_end::Reachable(factory)) {
            }

            void run(const std::vector<core::PieceType> &pieces, std::set<core::Bitboard> &fieldSet) {
                auto maxDepth = static_cast<int>(pieces.size());

                assert(1 <= maxDepth);

                // Copy field
                auto freeze = core::Field{};

                // Initialize moves
                std::vector<std::vector<core::Move>> movePools(maxDepth);
                for (int index = 0; index < maxDepth; ++index) {
                    movePools[index] = std::vector<core::Move>{};
                }

                // Initialize configure
                const Configure configure{
                        pieces,
                        movePools,
                        fieldSet,
                        maxDepth,
                        static_cast<int>(pieces.size()),
                };

                assert(configure.maxDepth == configure.pieceSize);

                // Execute
                search(configure, freeze, 0);
            }

        private:
            const core::Factory &factory_;
            T &moveGenerator_;
            core::srs_rotate_end::Reachable reachable_;

            void search(
                    const Configure &configure, const core::Field &field, int depth
            ) {
                auto &pieces = configure.pieces;
                auto &moves = configure.movePools[depth];

                auto &current = pieces[depth];

                moves.clear();
                if (depth < configure.maxDepth - 1) {
                    move(configure, field, moves, current, depth);
                } else {
                    moveLast(configure, field, moves, current, depth);
                }
            }

            void move(
                    const Configure &configure,
                    const core::Field &field,
                    std::vector<core::Move> &moves,
                    core::PieceType pieceType,
                    int depth
            ) {
                moveGenerator_.search(moves, field, pieceType, 4);

                for (const auto &move : moves) {
                    auto &blocks = factory_.get(pieceType, move.rotateType);

                    auto freeze = core::Field(field);
                    freeze.put(blocks, move.x, move.y);

                    int numCleared = freeze.clearLineReturnNum();

                    if (0 < numCleared) {
                        continue;
                    }

                    if (!validate(freeze, 4)) {
                        continue;
                    }

                    // Found new field
                    search(configure, freeze, depth + 1);
                }
            }

            void moveLast(
                    const Configure &configure,
                    const core::Field &field,
                    std::vector<core::Move> &moves,
                    core::PieceType pieceType,
                    int depth
            ) {
                moveGenerator_.search(moves, field, pieceType, 4);

                for (const auto &move : moves) {
                    auto &blocks = factory_.get(pieceType, move.rotateType);

                    auto freeze = core::Field(field);
                    freeze.put(blocks, move.x, move.y);

                    int numCleared = freeze.clearLineReturnNum();

                    if (0 < numCleared) {
                        continue;
                    }

                    if (!validate(freeze, 4)) {
                        continue;
                    }

                    configure.fieldSet.insert(freeze.boardLow());
                }
            }
        };
    }

    using namespace std::literals::string_literals;

    class SampleTest : public ::testing::Test {
    };

    struct Order {
        const uint64_t fromPieces;
        const uint64_t noUsedPieces;
        const uint64_t restPieceCounter;
    };

    inline bool operator<(const Order &lhs, const Order &rhs) {
        if (lhs.fromPieces != rhs.fromPieces) {
            return lhs.fromPieces < rhs.fromPieces;
        }

        if (lhs.noUsedPieces != rhs.noUsedPieces) {
            return lhs.noUsedPieces < rhs.noUsedPieces;
        }

        return lhs.restPieceCounter < rhs.restPieceCounter;
    }

    struct SearchTarget {
        const uint64_t fromPieces;
        const uint64_t restPieceCounter;
    };

    inline bool operator<(const SearchTarget &lhs, const SearchTarget &rhs) {
        if (lhs.fromPieces != rhs.fromPieces) {
            return lhs.fromPieces < rhs.fromPieces;
        }

        return lhs.restPieceCounter < rhs.restPieceCounter;
    }

    struct Result {
        const uint64_t field;
        const uint16_t success;
    };

    TEST_F(SampleTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = sfinder::perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto reverseLookup = sfinder::ReverseLookup::create(5, 6);

        std::cout << std::endl;

        auto permutation = sfinder::Permutation::create<7>(core::kAllPieceType, 6);
        auto lookup = sfinder::ForwardOrderLookUp::create(5, 6);

        auto allPieceCounter = core::PieceCounter::create<7>(core::kAllPieceType);

        // 6 pieces -> 5 pieces -> counter
        // multimap<5 pieces counter, no used pieces>
        auto map = std::map<uint64_t, std::set<Order>>{};
        for (int number = 0; number < permutation.indexSize(); ++number) {
            auto fromPieces = permutation.pieces(number);
            auto fromCounter6 = core::PieceCounter::create(fromPieces);

            auto rest = allPieceCounter - fromCounter6;
            assert(rest.vector().size() == 1);

            auto forward = lookup.parse(fromPieces);
            for (const auto &toPieces : forward) {
                auto to5 = core::Pieces::create(toPieces);
                auto toCounter5 = core::PieceCounter::create(to5.vector());

                auto noUsedCounter = fromCounter6 - toCounter5;
                auto noUsedPieces = core::Pieces::create(noUsedCounter.vector()); // TODO
                assert(noUsedPieces.size() == 1);

                map[toCounter5.value()].insert({to5.value(), noUsedPieces.value(), rest.value()});
            }
        }

        std::cout << map.size() << std::endl;
        for (const auto &item : map) {
            std::cout << "#" << std::endl;

            std::map<uint64_t, std::set<SearchTarget>> noUsedMap{};
            for (const auto &i : item.second) {
                noUsedMap[i.noUsedPieces].insert({i.fromPieces, i.restPieceCounter});
            }

            std::cout << "noUsedMap = " << noUsedMap.size() << std::endl;

            for (const auto &i : noUsedMap) {
                // Field
                auto fieldSet = std::set<core::Bitboard>{};
                for (const auto &second : i.second) {
                    auto to5 = core::Pieces(second.fromPieces, 5);
                    auto vector = to5.vector();

                    auto harddropMoveGenerator = core::harddrop::MoveGenerator(factory);
                    auto harddropFinder = MoveFinder(factory, harddropMoveGenerator);

                    harddropFinder.run(vector, fieldSet);
                }
                std::cout << "field = " << fieldSet.size() << std::endl;

                // rest
                auto restSet = std::set<uint64_t>{};
                for (const auto &second : i.second) {
                    restSet.insert(second.restPieceCounter);
                }
                std::cout << "rest = " << restSet.size() << std::endl;

                // percent
                auto noUsedPieces = core::Pieces(i.first, 1).vector();
                auto restPieces = core::Pieces(*restSet.begin(), 1).vector(); // TODO
                auto permutationVector = std::vector{
                        sfinder::Permutation::create(noUsedPieces, noUsedPieces.size()),
                        sfinder::Permutation::create(restPieces, restPieces.size()),
                        sfinder::Permutation::create<7>(core::kAllPieceType, 4)
                };
                auto permutations = sfinder::Permutations::create(permutationVector);
                auto percentage = sfinder::Percentage<>(finder, permutations, reverseLookup);

                int index = 0;
                auto totalTime = 0LL;
                auto max = -1.0;
                for (const auto &field : fieldSet) {
                    if (index % 1000 == 0 && index != 0) {
                        std::cout << " " << index << " : average = " << static_cast<double>(totalTime) / index << "ms"
                                  << std::endl;
                    }
                    index += 1;

                    auto start = std::chrono::system_clock::now();

                    auto p = percentage.run(field, 5, 4);

                    auto elapsed = std::chrono::system_clock::now() - start;
                    totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

                    if (max < p) {
                        max = p;
                    }
                }

                std::cout << "max = " << max << std::endl;
            }
        }
    }

    sfinder::Permutations createHead6PermutationsWithoutHold(int cycle) {
        switch (cycle) {
            case 1: {
                auto vector = std::vector{
                        sfinder::Permutation::create<7>(core::kAllPieceType, 6)
                };
                return sfinder::Permutations::create(vector);
            }
            case 4: {
                auto vector = std::vector{
                        sfinder::Permutation::create<7>(core::kAllPieceType, 5),
                        sfinder::Permutation::create<7>(core::kAllPieceType, 1)
                };
                return sfinder::Permutations::create(vector);
            }
            default: {
                assert(false);
                auto vector = std::vector<sfinder::Permutation>{};
                return sfinder::Permutations::create(vector);
            }
        }
    }

    int getLastPermutationStartIndex(int cycle) {
        switch (cycle) {
            case 1:
                return 0;
            case 4:
                return 5;
            default: {
                assert(false);
                return -1;
            }
        }
    }

    void withoutHold(int cycle) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto pcFinder = sfinder::perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto harddropMoveGenerator = core::harddrop::MoveGenerator(factory);
        auto moveFinder = MoveFinder(factory, harddropMoveGenerator);

        auto permutations = createHead6PermutationsWithoutHold(cycle);

        auto forwardLookup = sfinder::ForwardOrderLookUp::create(5, 6);
        auto reverseLookup = sfinder::ReverseLookup::create(5, 6);

        auto allPieceCounter = core::PieceCounter::create<7>(core::kAllPieceType);

        auto lastPermutationStartIndex = getLastPermutationStartIndex(cycle);
        auto noUsedSize = 1;  // holdなしのときは必ず1

        // 6 pieces -> 5 pieces -> counter
        // multimap<5 pieces counter, no used pieces>
        auto each5PieceCounter = std::map<uint64_t, std::set<Order>>{};
        for (int number = 0; number < permutations.size(); ++number) {
            auto fromPieces = permutations.pieces(number);

            // 最後のブロックで使っていないPiece
            auto restPieceVector = std::vector<core::PieceType>{};
            for (auto p = fromPieces.begin() + lastPermutationStartIndex; p < fromPieces.end(); p += 1) {
                restPieceVector.emplace_back(*p);
            }
            auto restPieceCounter = allPieceCounter - core::PieceCounter::create(restPieceVector);

            auto fromCounter6 = core::PieceCounter::create(fromPieces);

            // 5ミノの組み合わせ -> (5ミノの並び, 使っていないミノの並び, 5ミノ以降のミノの組み合わせ)
            auto forward = forwardLookup.parse(fromPieces);
            for (const auto &toPieces : forward) {
                auto to5 = core::Pieces::create(toPieces);
                auto toCounter5 = core::PieceCounter::create(to5.vector());

                auto noUsedCounter = fromCounter6 - toCounter5;
                auto noUsedPieces = core::Pieces::create(noUsedCounter.vector());
                assert(noUsedPieces.size() == noUsedSize);

                each5PieceCounter[toCounter5.value()].insert(
                        {to5.value(), noUsedPieces.value(), restPieceCounter.value()}
                );
            }
        }

        std::cout << "map = " << each5PieceCounter.size() << std::endl;

        for (const auto &[pieceCounter5, orders] : each5PieceCounter) {
            // ファイル名の生成
            auto fileName = ""s;
            for (const auto &piece : core::PieceCounter(pieceCounter5).vector()) {
                fileName += factory.get(piece).name;
            }

            std::cout << "#" << std::endl;

            // 使っていないミノ -> (ツモ, ネクスト以降のミノの組み合わせ)
            std::map<uint64_t, std::set<SearchTarget>> noUsedMap{};
            for (const auto &order : orders) {
                noUsedMap[order.noUsedPieces].insert({order.fromPieces, order.restPieceCounter});
            }

            std::cout << "noUsedMap = " << noUsedMap.size() << std::endl;

            // Field
            auto fieldSet = std::set<core::Bitboard>{};

            for (const auto &order : orders) {
                auto to5 = core::Pieces(order.fromPieces, 5);
                auto vector = to5.vector();
                moveFinder.run(vector, fieldSet);
            }

            std::cout << "fieldSet = " << fieldSet.size() << std::endl;

            for (const auto &[noUsedPieces, searchTargets] : noUsedMap) {
                // rest
                auto restSet = std::set<uint64_t>{};
                for (const auto &searchTarget : searchTargets) {
                    restSet.insert(searchTarget.restPieceCounter);
                }

                std::cout << "restSet = " << restSet.size() << std::endl;
                assert(restSet.size() == 1);

                const auto restPieceCounter = *restSet.begin();

                // percent
                auto noUsedPiecesVector = core::Pieces(noUsedPieces, noUsedSize).vector();
                auto restPiecesVector = core::PieceCounter(restPieceCounter).vector();

                assert(noUsedPiecesVector.size() == 1);
                assert(!restPiecesVector.empty());

                std::vector<sfinder::Permutation> permutationVector;
                if (restPiecesVector.size() < 5) {
                    permutationVector = std::vector{
                            sfinder::Permutation::create<1>(
                                    std::array<core::PieceType, 1>{noUsedPiecesVector[0]}, 1
                            ),
                            sfinder::Permutation::create(restPiecesVector, restPiecesVector.size()),
                            sfinder::Permutation::create<7>(core::kAllPieceType,
                                                            6 - (static_cast<int>(restPiecesVector.size()) + 1))
                    };
                } else {
                    permutationVector = std::vector{
                            sfinder::Permutation::create<1>(
                                    std::array<core::PieceType, 1>{noUsedPiecesVector[0]}, 1
                            ),
                            sfinder::Permutation::create(restPiecesVector, 5),
                    };
                }

                auto pcPermutations = sfinder::Permutations::create(permutationVector);

                std::cout << "permutations = " << pcPermutations.size() << std::endl;

                auto percentage = sfinder::Percentage<>(pcFinder, pcPermutations, reverseLookup);

                auto resultVector = std::vector<Result>{};
                resultVector.reserve(fieldSet.size());

                int index = 0;
                auto totalTime = 0LL;
                auto max = -1.0;
                auto mf = core::Field{};
                for (const auto &field : fieldSet) {
                    if (index % 1000 == 0 && index != 0) {
                        std::cout << " " << index << " : average = " << static_cast<double>(totalTime) / index
                                  << "ms"
                                  << std::endl;
                    }
                    index += 1;

                    auto start = std::chrono::system_clock::now();

                    uint16_t p = percentage.run(field, 5, 4);

                    auto elapsed = std::chrono::system_clock::now() - start;
                    totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

                    if (max < p) {
                        max = p;
                        mf = core::Field(field);
                    }

                    resultVector.push_back({field, p});
                }

                std::cout << "max = " << max << std::endl;
                std::cout << "max.p = " << static_cast<double>(max) / pcPermutations.size() << std::endl;
                std::cout << mf.toString(4) << std::endl;

                // ファイル名の生成
                auto fileName2 = ""s;
                for (const auto &piece : noUsedPiecesVector) {
                    fileName2 += factory.get(piece).name;
                }

                std::ofstream ofstream(
                        std::to_string(cycle).append("/").append(fileName).append("_").append(fileName2),
                        std::ios::binary | std::ios::out | std::ios::trunc
                );

                if (!ofstream) {
                    std::cout << "Cannot open file";
                    return;
                }

                {
                    uint32_t val = pcPermutations.size();
                    ofstream.write(reinterpret_cast<char *>(&val), sizeof(uint32_t));
                }

                {
                    uint32_t val = fieldSet.size();
                    ofstream.write(reinterpret_cast<char *>(&val), sizeof(uint32_t));
                }

                for (const auto result : resultVector) {
                    uint64_t field = result.field;
                    uint16_t success = result.success;
                    ofstream.write(reinterpret_cast<char *>(&field), sizeof(uint64_t));
                    ofstream.write(reinterpret_cast<char *>(&success), sizeof(uint16_t));
                }

                ofstream.close();
            }
        }
    }

    TEST_F(SampleTest, noHold) {
        // 4回目: *p5, *p6
        std::cout << std::endl;

        withoutHold(1);
    }
}
