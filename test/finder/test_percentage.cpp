#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

#include "finder/percentage.hpp"

namespace finder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class PercentageTest : public ::testing::Test {
    };

    TEST_F(PercentageTest, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<7>(core::kAllPieceType, 7)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 7;
        const int maxLine = 6;

        auto reverseLookup = ReverseLookup::create(maxDepth, 7);

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

        EXPECT_EQ(success, 3248);
    }
    TEST_F(PercentageTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto permutationVector = std::vector{
                Permutation::create<7>(core::kAllPieceType, 4)
        };
        auto permutations = Permutations::create(permutationVector);

        const int maxDepth = 3;
        const int maxLine = 4;

        auto reverseLookup = ReverseLookup::create(maxDepth, 4);

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
}
