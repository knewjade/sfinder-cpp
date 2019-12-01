#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>
#include <array>

#include "sfinder/percentage.hpp"

namespace sfinder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class PercentageTest : public ::testing::Test {
    };

    TEST_F(PercentageTest, line6case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<7>(core::kAllPieceType, 7)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 7;
        const int maxLine = 6;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 7);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );

        int success = percentage.run(field, maxDepth, maxLine);
        EXPECT_EQ(permutations.size(), 5040);
        EXPECT_EQ(success, 5038);
    }

    TEST_F(PercentageTest, line6case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<7>(core::kAllPieceType, 7)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 7;
        const int maxLine = 6;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 7);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "__________"s +
                "_XX_______"s +
                "XXXXX____X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );

        int success = percentage.run(field, maxDepth, maxLine);
        EXPECT_EQ(permutations.size(), 5040);
        EXPECT_EQ(success, 3248);
    }

    TEST_F(PercentageTest, pco) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<7>(core::kAllPieceType, 4)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 3;
        const int maxLine = 4;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 4);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "XXXX____XX"s +
                "XXXX___XXX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );
        int success = percentage.run(field, maxDepth, maxLine);

        EXPECT_EQ(permutations.size(), 840);
        EXPECT_EQ(success, 514);
    }

    TEST_F(PercentageTest, pcoHoldI) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<1>(std::array<core::PieceType, 1>{core::PieceType::I}, 1),
                Permutation::create<7>(core::kAllPieceType, 4)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 4;
        const int maxLine = 4;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 5);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "XX_____XXX"s +
                "XXX____XXX"s +
                "XXXX___XXX"s +
                "XXX____XXX"s +
                ""
        );
        int success = percentage.run(field, maxDepth, maxLine);

        EXPECT_EQ(permutations.size(), 840);
        EXPECT_EQ(success, 711);
    }

    TEST_F(PercentageTest, graceSystem) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<1>(std::array<core::PieceType, 1>{core::PieceType::T}, 1),
                Permutation::create<7>(core::kAllPieceType, 4)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 4;
        const int maxLine = 4;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 5);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "XXXXXX____"s +
                "XXXXXX____"s +
                "XXXXXX____"s +
                "XXXXXX____"s +
                ""
        );
        int success = percentage.run(field, maxDepth, maxLine);

        EXPECT_EQ(permutations.size(), 840);
        EXPECT_EQ(success, 744);
    }

    TEST_F(PercentageTest, depth5case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::PerfectClearFinder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<2>(std::array<core::PieceType, 2>{core::PieceType::L, core::PieceType::J}, 2),
                Permutation::create<7>(core::kAllPieceType, 4)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 5;
        const int maxLine = 4;

        auto reverseLookup = ReverseOrderLookup::create(maxDepth, 6);

        auto percentage = Percentage<>(finder, permutations, reverseLookup);

        auto field = core::createField(
                "XX_______X"s +
                "XXX______X"s +
                "XXXX___XXX"s +
                "XXX____XXX"s +
                ""
        );
        int success = percentage.run(field, maxDepth, maxLine);

        EXPECT_EQ(permutations.size(), 1680);
        EXPECT_EQ(success, 1550);
    }
}
