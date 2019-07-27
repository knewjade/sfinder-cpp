#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <vector>

#include "core/pieces.hpp"

namespace core {
    using namespace ::testing;

    class PiecesTest : public ::testing::Test {
    };

    namespace {
        TEST_F(PiecesTest, pieces) {
            auto pieces = Pieces::create(std::vector<core::PieceType>{
                    core::PieceType::T, core::PieceType::I, core::PieceType::O,
            });
            auto vector = pieces.vector();
            ASSERT_THAT(vector, ElementsAre(core::PieceType::T, core::PieceType::I, core::PieceType::O));
        }
    }

    namespace {
        TEST_F(PiecesTest, pieceCounter) {
            auto counter = PieceCounter::create(std::vector<core::PieceType>{
                    core::PieceType::T, core::PieceType::T, core::PieceType::T, core::PieceType::T,
                    core::PieceType::I, core::PieceType::I, core::PieceType::I,
                    core::PieceType::O, core::PieceType::O,
                    core::PieceType::Z,
            });

            EXPECT_EQ(counter[core::PieceType::T], 4);
            EXPECT_EQ(counter[core::PieceType::I], 3);
            EXPECT_EQ(counter[core::PieceType::O], 2);
            EXPECT_EQ(counter[core::PieceType::L], 0);
            EXPECT_EQ(counter[core::PieceType::J], 0);
            EXPECT_EQ(counter[core::PieceType::S], 0);
            EXPECT_EQ(counter[core::PieceType::Z], 1);

            ASSERT_THAT(counter.vector(), ElementsAre(
                    core::PieceType::T, core::PieceType::T, core::PieceType::T, core::PieceType::T,
                    core::PieceType::I, core::PieceType::I, core::PieceType::I,
                    core::PieceType::Z,
                    core::PieceType::O, core::PieceType::O
            ));
        }
    }
}
