#include "gtest/gtest.h"

#include "finder/two_lines_pc.hpp"

namespace finder {
    using namespace std::literals::string_literals;

    class TwoLinesPCTest : public ::testing::Test {
    };

    TEST_F(TwoLinesPCTest, 5pieces) {
        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::O,
                    core::PieceType::L,
                    core::PieceType::I,
                    core::PieceType::J,
                    core::PieceType::O,
            };
            EXPECT_TRUE(canTake2LinePC(pieces));
        }

        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::I,
                    core::PieceType::J,
                    core::PieceType::I,
                    core::PieceType::J,
                    core::PieceType::O,
            };
            EXPECT_TRUE(canTake2LinePC(pieces));
        }

        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::I,
                    core::PieceType::J,
                    core::PieceType::T,
                    core::PieceType::J,
                    core::PieceType::O,
            };
            EXPECT_FALSE(canTake2LinePC(pieces));
        }
    }

    TEST_F(TwoLinesPCTest, 6pieces) {
        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::O,
                    core::PieceType::L,
                    core::PieceType::S,
                    core::PieceType::I,
                    core::PieceType::J,
                    core::PieceType::O,
            };
            EXPECT_TRUE(canTake2LinePC(pieces));
        }

        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::I,
                    core::PieceType::S,
                    core::PieceType::L,
                    core::PieceType::I,
                    core::PieceType::I,
                    core::PieceType::J,
            };
            EXPECT_TRUE(canTake2LinePC(pieces));
        }

        {
            auto pieces = std::vector<core::PieceType> {
                    core::PieceType::S,
                    core::PieceType::I,
                    core::PieceType::O,
                    core::PieceType::T,
                    core::PieceType::I,
                    core::PieceType::O,
            };
            EXPECT_FALSE(canTake2LinePC(pieces));
        }
    }
}
