#include "gtest/gtest.h"

#include <chrono>

#include "core/field.hpp"
#include "core/moves.hpp"
#include "sfinder/spins.hpp"
#include "sfinder/permutations.hpp"
#include "sfinder/perfect_clear/checker.hpp"

namespace sfinder::perfect_clear {
    using namespace std::literals::string_literals;

    class CheckerTest : public ::testing::Test {
    };

    TEST_F(CheckerTest, longtest1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = Checker<core::srs::MoveGenerator>(factory, moveGenerator);

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

        auto permutations = Permutation::create<7>(core::kAllPieceType, 7);
        int max = permutations.indexSize();

        int success = 0;
        long long int totalTime = 0L;
        for (int value = 0; value < max; ++value) {
            auto pieces = permutations.pieces(value);

            auto start = std::chrono::system_clock::now();

            auto result = finder.run<true>(field, pieces, maxDepth, maxLine, false);

            // Failed: 975, 2295
            if (result) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 5038);
    }

    TEST_F(CheckerTest, longtest2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = Checker<core::srs::MoveGenerator>(factory, moveGenerator);

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

        auto permutations = Permutation::create<7>(core::kAllPieceType, 7);
        int max = permutations.indexSize();

        int success = 0;
        long long int totalTime = 0L;
        for (int value = 0; value < max; ++value) {
            auto pieces = permutations.pieces(value);

            auto start = std::chrono::system_clock::now();

            auto result = finder.run<true>(field, pieces, maxDepth, maxLine, false);

            if (result) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 3248);
    }

    TEST_F(CheckerTest, longtest3) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = Checker<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXX____XX"s +
                "XXXX___XXX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );
        const int maxDepth = 3;
        const int maxLine = 4;

        auto permutations = Permutation::create<7>(core::kAllPieceType, 4);
        int max = permutations.indexSize();

        int success = 0;
        long long int totalTime = 0L;
        for (int value = 0; value < max; ++value) {
            auto pieces = permutations.pieces(value);

            auto start = std::chrono::system_clock::now();

            auto result = finder.run<true>(field, pieces, maxDepth, maxLine, false);

            if (result) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 514);
    }

    TEST_F(CheckerTest, longtest4) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = Checker<core::srs::MoveGenerator>(factory, moveGenerator);

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

        auto permutations = Permutation::create<7>(core::kAllPieceType, 7);
        int max = permutations.indexSize();

        int success = 0;
        long long int totalTime = 0L;
        for (int value = 0; value < max; ++value) {
            auto pieces = permutations.pieces(value);

            auto start = std::chrono::system_clock::now();

            auto result = finder.run<true>(field, pieces, maxDepth, maxLine, false);

            if (result) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 3248);
    }
}
