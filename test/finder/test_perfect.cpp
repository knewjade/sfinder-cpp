#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"
#include "finder/perfect.hpp"

namespace finder {
    using namespace std::literals::string_literals;

    class PerfectTest : public ::testing::Test {
    };

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(PerfectTest, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(result);
        }
    }

    // depth = 1 && piece = 2 && first hold exists
    TEST_F(PerfectTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::T, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(result);
        }
    }

    // depth = 3 && piece = 3 && first hold is empty
    TEST_F(PerfectTest, case3) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(result);
        }
    }

    // depth = 3 && piece = 4 && first hold is empty
    TEST_F(PerfectTest, case4) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(result);
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectTest, case5) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::O, core::PieceType::J, core::PieceType::S, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::Z, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::L, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(result);
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectTest, block6) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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
            EXPECT_FALSE(result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::S, core::PieceType::J, core::PieceType::L, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::I, core::PieceType::T
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(result);
        }
    }

    TEST_F(PerfectTest, measure1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

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

        long long int sum = 0;
        for (int count = 0; count < 100; ++count) {
            auto start = std::chrono::system_clock::now();

            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::I, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);

            auto end = std::chrono::system_clock::now();

            auto diff = end - start;
            sum += std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

            EXPECT_FALSE(result);
        }

        std::cout << "elapsed time = " << (sum / 100.0) << " msec." << std::endl;
    }

    TEST_F(PerfectTest, measure2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX_____"s +
                "XXXXX_____"s +
                "XXXXX____X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        auto maxDepth = 5;
        auto maxLine = 6;

        long long int sum = 0;
        int max = 100;
        for (int count = 0; count < max; ++count) {
            auto start = std::chrono::system_clock::now();

            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);

            auto end = std::chrono::system_clock::now();

            auto diff = end - start;
            sum += std::chrono::duration_cast<std::chrono::microseconds>(diff).count();

            EXPECT_TRUE(result);
        }

        std::cout << "elapsed time = " << (sum / static_cast<double>(max)) << " microsec." << std::endl;
    }
}
