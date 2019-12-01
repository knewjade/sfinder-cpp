#include <chrono>

#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"
#include "sfinder/perfect_clear/full_finder.hpp"

void verify(const core::Factory &factory, const core::Field &field, const sfinder::perfect_clear::Solution &solution) {
    auto freeze = core::Field(field);

    for (const auto &operation : solution) {
        auto &blocks = factory.get(operation.pieceType, operation.rotateType);
        EXPECT_TRUE(freeze.canPut(blocks, operation.x, operation.y));
        freeze.put(blocks, operation.x, operation.y);
        freeze.clearLine();

//        std::cout << freeze.toString(4) << std::endl;
    }

    EXPECT_EQ(freeze, core::Field{});
}

namespace sfinder::perfect_clear {
    using namespace std::literals::string_literals;

    class PerfectClearTest : public ::testing::Test {
    };

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(PerfectClearTest, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxDepth = 1;
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::S};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 1 && piece = 2 && first hold exists
    TEST_F(PerfectClearTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxDepth = 1;
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::Z, core::PieceType::S};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::T, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 3 && first hold is empty
    TEST_F(PerfectClearTest, case3) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold is empty
    TEST_F(PerfectClearTest, case4) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectClearTest, case5) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::T, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::O, core::PieceType::J, core::PieceType::S, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::Z, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::L, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectClearTest, case6) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        auto maxDepth = 7;
        auto maxLine = 6;

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::I, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::S, core::PieceType::J, core::PieceType::L, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::I, core::PieceType::T
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }
    }

    template<int N>
    std::array<core::PieceType, N> toPieces(int value) {
        int arr[N];

        for (int index = N - 1; 0 <= index; --index) {
            int scale = 7 - index;
            arr[index] = value % scale;
            value /= scale;
        }

        for (int select = N - 2; 0 <= select; --select) {
            for (int adjust = select + 1; adjust < N; ++adjust) {
                if (arr[select] <= arr[adjust]) {
                    arr[adjust] += 1;
                }
            }
        }

        std::array<core::PieceType, N> pieces = {};
        for (int index = 0; index < N; ++index) {
            pieces[index] = static_cast<core::PieceType>(arr[index]);
        }

        return pieces;
    }

    TEST_F(PerfectClearTest, longtest1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        const int maxDepth = 7;
        const int maxLine = 6;

        int success = 0;
        long long int totalTime = 0L;
        int max = 5040;
        for (int value = 0; value < max; ++value) {
            auto arr = toPieces<maxDepth>(value);
            auto pieces = std::vector(arr.begin(), arr.end());

            auto start = std::chrono::system_clock::now();

            auto result = finder.run(field, pieces, maxDepth, maxLine, false, false, 3, SearchTypes::TSpin, false);

            // Failed: 975, 2295
            if (!result.empty()) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 5038);
    }

    TEST_F(PerfectClearTest, longtest2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "__________"s +
                "_XX_______"s +
                "XXXXX____X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        const int maxDepth = 7;
        const int maxLine = 6;

        int success = 0;
        long long int totalTime = 0L;
        int max = 5040;
        for (int value = 0; value < max; ++value) {
            auto arr = toPieces<maxDepth>(value);
            auto pieces = std::vector(arr.begin(), arr.end());

            auto start = std::chrono::system_clock::now();

            auto result = finder.run(field, pieces, maxDepth, maxLine, false, true, 0, SearchTypes::TSpin, false);

            if (!result.empty()) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 3248);
    }
}

namespace sfinder::perfect_clear {
    using namespace std::literals::string_literals;

    class PerfectClearTest2 : public ::testing::Test {
    };

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(PerfectClearTest2, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(""s +
                                       "XXXXX__XXX"s +
                                       "XXXX__XXXX"s +
                                       ""
        );
        auto maxDepth = 1;
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::S};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 0, true, 0);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 0, true, 0);
            EXPECT_FALSE(!result.empty());
        }
    }

    TEST_F(PerfectClearTest2, searchFast) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(""s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       ""
        );
        auto maxDepth = 9;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::T, core::PieceType::Z, core::PieceType::O, core::PieceType::J,
                                      core::PieceType::L, core::PieceType::I, core::PieceType::Z, core::PieceType::T, core::PieceType::J};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 0, true, 0);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);

            auto freeze = core::Field(field);

            int clearLineCount = 0;
            for (const auto &operation : result) {
                auto &blocks = factory.get(operation.pieceType, operation.rotateType);
                freeze.put(blocks, operation.x, operation.y);
                auto clearLine = freeze.clearLineReturnNum();
                if (0 < clearLine) {
                    clearLineCount += 1;
                }
            }

            EXPECT_EQ(clearLineCount, 2);
        }
    }

    TEST_F(PerfectClearTest2, searchTSpin) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(""s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       ""
        );
        auto maxDepth = 9;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::T, core::PieceType::Z, core::PieceType::O, core::PieceType::J,
                                      core::PieceType::L, core::PieceType::I, core::PieceType::Z, core::PieceType::T, core::PieceType::J};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 1, true, 0);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);

            auto freeze = core::Field(field);

            auto reachable = core::srs_rotate_end::Reachable(factory);
            int attackCount = 0;
            for (const auto &operation : result) {
                auto piece = operation.pieceType;
                auto rotate = operation.rotateType;
                int x = operation.x;
                int y = operation.y;
                auto &blocks = factory.get(piece, rotate);

                auto beforeClear = core::Field(freeze);

                freeze.put(blocks, x, y);
                auto clearLine = freeze.clearLineReturnNum();

                auto attacks = getAttackIfTSpin(reachable, factory, beforeClear, piece, {rotate, x, y, freeze.canReachOnHarddrop(blocks, x, y)}, clearLine, false);
                if (0 < attacks) {
                    attackCount += 1;
                }
            }

            EXPECT_EQ(attackCount, 1);
        }
    }

    TEST_F(PerfectClearTest2, searchAllSpins) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(""s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       "X_________"s +
                                       ""
        );
        auto maxDepth = 9;
        auto maxLine = 4;

        // no mini (all spins are judged as regular attack)
        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::T, core::PieceType::Z, core::PieceType::O, core::PieceType::J,
                                      core::PieceType::L, core::PieceType::I, core::PieceType::Z, core::PieceType::T, core::PieceType::J};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 2, true, 0);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);

            auto freeze = core::Field(field);

            int attackCountAlwaysRegular = 0;
            int attackCountWithoutMini = 0;
            for (const auto &operation : result) {
                auto piece = operation.pieceType;
                auto rotate = operation.rotateType;
                int x = operation.x;
                int y = operation.y;
                auto &blocks = factory.get(piece, rotate);

                auto beforeClear = core::Field(freeze);

                freeze.put(blocks, x, y);
                auto clearLine = freeze.clearLineReturnNum();

                {
                    auto attacksAlwaysRegular = getAttackIfAllSpins<true>(factory, beforeClear, piece, {rotate, x, y, freeze.canReachOnHarddrop(blocks, x, y)}, clearLine, false);
                    if (0 < attacksAlwaysRegular) {
                        attackCountAlwaysRegular += 1;
                    }
                }

                {
                    auto attackWithoutMini = getAttackIfAllSpins<false>(factory, beforeClear, piece, {rotate, x, y, freeze.canReachOnHarddrop(blocks, x, y)}, clearLine, false);
                    if (0 < attackWithoutMini) {
                        attackCountWithoutMini += 1;
                    }
                }
            }

            EXPECT_EQ(attackCountAlwaysRegular, 2);
            EXPECT_EQ(attackCountWithoutMini, 1);
        }

        // with mini (mini is zero attack)
        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::T, core::PieceType::Z, core::PieceType::O, core::PieceType::J,
                                      core::PieceType::L, core::PieceType::I, core::PieceType::Z, core::PieceType::T, core::PieceType::J};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true, 3, true, 0);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);

            auto freeze = core::Field(field);

            int attackCountAlwaysRegular = 0;
            int attackCountWithoutMini = 0;
            for (const auto &operation : result) {
                auto piece = operation.pieceType;
                auto rotate = operation.rotateType;
                int x = operation.x;
                int y = operation.y;
                auto &blocks = factory.get(piece, rotate);

                auto beforeClear = core::Field(freeze);

                freeze.put(blocks, x, y);
                auto clearLine = freeze.clearLineReturnNum();

                {
                    auto attacksAlwaysRegular = getAttackIfAllSpins<true>(factory, beforeClear, piece, {rotate, x, y, freeze.canReachOnHarddrop(blocks, x, y)}, clearLine, false);
                    if (0 < attacksAlwaysRegular) {
                        attackCountAlwaysRegular += 1;
                    }
                }

                {
                    auto attackWithoutMini = getAttackIfAllSpins<false>(factory, beforeClear, piece, {rotate, x, y, freeze.canReachOnHarddrop(blocks, x, y)}, clearLine, false);
                    if (0 < attackWithoutMini) {
                        attackCountWithoutMini += 1;
                    }
                }
            }

            EXPECT_EQ(attackCountAlwaysRegular, 2);
            EXPECT_EQ(attackCountWithoutMini, 2);
        }
    }
}