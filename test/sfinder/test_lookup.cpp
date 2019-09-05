#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

#include "sfinder/lookup.hpp"

namespace sfinder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class LookupTest : public ::testing::Test {
    };

    namespace {
        TEST_F(LookupTest, reverse1) {
            auto lookup = ReverseOrderLookup::create(2, 3);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 4);
            EXPECT_EQ(parse.at(0).size(), 3);

            ASSERT_THAT(parse.at(0), ElementsAre(-1, core::PieceType::T, core::PieceType::I));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::T, -1, core::PieceType::I));
            ASSERT_THAT(parse.at(2), ElementsAre(core::PieceType::I, core::PieceType::T, -1));
            ASSERT_THAT(parse.at(3), ElementsAre(core::PieceType::T, core::PieceType::I, -1));
        }

        TEST_F(LookupTest, reverse2) {
            auto lookup = ReverseOrderLookup::create(3, 3);
            auto vector = std::vector{core::PieceType::S, core::PieceType::Z, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 4);
            EXPECT_EQ(parse.at(0).size(), 3);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::O, core::PieceType::S, core::PieceType::Z));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::S, core::PieceType::O, core::PieceType::Z));
            ASSERT_THAT(parse.at(2), ElementsAre(core::PieceType::Z, core::PieceType::S, core::PieceType::O));
            ASSERT_THAT(parse.at(3), ElementsAre(core::PieceType::S, core::PieceType::Z, core::PieceType::O));
        }
    }

    namespace {
        TEST_F(LookupTest, forward_holdEmpty1) {
            auto lookup = ForwardOrderLookUp::create(2, 3, true, false);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 4);
            EXPECT_EQ(parse.at(0).size(), 2);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::T, core::PieceType::O));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::I, core::PieceType::O));
            ASSERT_THAT(parse.at(2), ElementsAre(core::PieceType::T, core::PieceType::I));
            ASSERT_THAT(parse.at(3), ElementsAre(core::PieceType::I, core::PieceType::T));
        }

        TEST_F(LookupTest, forward_holdEmpty_mustUseHoldAtFirst) {
            auto lookup = ForwardOrderLookUp::create(2, 3, true, true);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 2);
            EXPECT_EQ(parse.at(0).size(), 2);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::I, core::PieceType::O));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::I, core::PieceType::T));
        }

        TEST_F(LookupTest, forward_holdNotEmpty1) {
            auto lookup = ForwardOrderLookUp::create(2, 3, false, false);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 4);
            EXPECT_EQ(parse.at(0).size(), 2);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::I, core::PieceType::O));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::T, core::PieceType::O));
            ASSERT_THAT(parse.at(2), ElementsAre(core::PieceType::I, core::PieceType::T));
            ASSERT_THAT(parse.at(3), ElementsAre(core::PieceType::T, core::PieceType::I));
        }

        TEST_F(LookupTest, forward_holdNotEmpty1_mustUseHoldAtFirst) {
            auto lookup = ForwardOrderLookUp::create(2, 3, false, true);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 2);
            EXPECT_EQ(parse.at(0).size(), 2);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::T, core::PieceType::O));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::T, core::PieceType::I));
        }
    }
}
