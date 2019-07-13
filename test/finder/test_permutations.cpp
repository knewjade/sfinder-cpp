#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <array>

#include "finder/permutation.hpp"

namespace finder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class PermutationsTest : public ::testing::Test {
    };

    TEST_F(PermutationsTest, TI) {
        auto pieces = std::vector<core::PieceType>{
                core::PieceType::T, core::PieceType::I
        };
        auto combinations = Permutation(pieces, 1);

        EXPECT_EQ(combinations.size(), 2);
        ASSERT_THAT(combinations.pieces(0), ElementsAre(core::PieceType::T));
        ASSERT_THAT(combinations.pieces(1), ElementsAre(core::PieceType::I));
    }

    TEST_F(PermutationsTest, SZO) {
        auto pieces = std::vector<core::PieceType>{
                core::PieceType::S, core::PieceType::Z, core::PieceType::O
        };
        auto combinations = Permutation(pieces, 2);

        EXPECT_EQ(combinations.size(), 6);

        {
            auto vector = std::vector<core::PieceType>{};
            combinations.push(vector, 0);
            ASSERT_THAT(vector, ElementsAre(core::PieceType::S, core::PieceType::Z));
        }

        {
            auto vector = std::vector<core::PieceType>{};
            combinations.push(vector, 5);
            ASSERT_THAT(vector, ElementsAre(core::PieceType::O, core::PieceType::Z));
        }
    }

    TEST_F(PermutationsTest, allPieceType) {
        auto combinations = Permutation::create<7>(core::kAllPieceType, 3);

        EXPECT_EQ(combinations.size(), 7 * 6 * 5);
        ASSERT_THAT(combinations.pieces(0), ElementsAre(core::PieceType::T, core::PieceType::I, core::PieceType::L));
        ASSERT_THAT(combinations.pieces(1), ElementsAre(core::PieceType::T, core::PieceType::I, core::PieceType::J));
        ASSERT_THAT(combinations.pieces(209), ElementsAre(core::PieceType::O, core::PieceType::Z, core::PieceType::S));
    }
}
