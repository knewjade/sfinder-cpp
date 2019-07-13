#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "finder/cycle.hpp"

namespace finder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class CycleTest : public ::testing::Test {
    };

    TEST_F(CycleTest, cycle1) {
        auto cycle = Cycle::create(1);
        EXPECT_EQ(cycle.size(), 5040 * 840);

        ASSERT_THAT(cycle.pieces(0), ElementsAre(
                core::PieceType::T, core::PieceType::I, core::PieceType::L,
                core::PieceType::J, core::PieceType::S, core::PieceType::Z, core::PieceType::O,
                core::PieceType::T, core::PieceType::I, core::PieceType::L, core::PieceType::J
        ));

        ASSERT_THAT(cycle.pieces(5040 * 840 - 1), ElementsAre(
                core::PieceType::O, core::PieceType::Z, core::PieceType::S, core::PieceType::J,
                core::PieceType::L, core::PieceType::I, core::PieceType::T,
                core::PieceType::O, core::PieceType::Z, core::PieceType::S, core::PieceType::J
        ));
    }
}
