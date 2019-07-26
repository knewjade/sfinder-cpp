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
            auto lookup = ReverseLookup::create(2, 3);
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
            auto lookup = ReverseLookup::create(3, 3);
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
        TEST_F(LookupTest, forward1) {
            auto lookup = ForwardOrderLookUp::create(2, 3);
            auto vector = std::vector{core::PieceType::T, core::PieceType::I, core::PieceType::O};

            auto parse = lookup.parse(vector);
            EXPECT_EQ(parse.size(), 4);
            EXPECT_EQ(parse.at(0).size(), 2);

            ASSERT_THAT(parse.at(0), ElementsAre(core::PieceType::T, core::PieceType::O));
            ASSERT_THAT(parse.at(1), ElementsAre(core::PieceType::I, core::PieceType::O));
            ASSERT_THAT(parse.at(2), ElementsAre(core::PieceType::T, core::PieceType::I));
            ASSERT_THAT(parse.at(3), ElementsAre(core::PieceType::I, core::PieceType::T));
        }
    }
}
