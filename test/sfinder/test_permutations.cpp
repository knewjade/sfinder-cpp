#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <queue>
#include <deque>

#include "sfinder/permutations.hpp"

namespace sfinder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    namespace {  // Permutations
        class PermutationsTest : public ::testing::Test {
        };

        TEST_F(PermutationsTest, cycle1) {
            auto permutation = Permutations::createCycle(1);
            EXPECT_EQ(permutation.size(), 5040 * 840);
            EXPECT_EQ(permutation.depth(), 11);

            ASSERT_THAT(permutation.pieces(0), ElementsAre(
                    core::PieceType::T, core::PieceType::I, core::PieceType::L,
                    core::PieceType::J, core::PieceType::S, core::PieceType::Z, core::PieceType::O,
                    core::PieceType::T, core::PieceType::I, core::PieceType::L, core::PieceType::J
            ));

            ASSERT_THAT(permutation.pieces(5040 * 840 - 1), ElementsAre(
                    core::PieceType::O, core::PieceType::Z, core::PieceType::S, core::PieceType::J,
                    core::PieceType::L, core::PieceType::I, core::PieceType::T,
                    core::PieceType::O, core::PieceType::Z, core::PieceType::S, core::PieceType::J
            ));

            for (int number = 0; number < permutation.size(); ++number) {
                auto pieces = permutation.pieces(number);
                ASSERT_EQ(permutation.numberify(pieces), number);
            }

            EXPECT_EQ(permutation.scale(0), 5040 * 840);
            EXPECT_EQ(permutation.scale(1), 5040 * 840 / 7);
            EXPECT_EQ(permutation.scale(2), 5040 * 840 / 42);
            EXPECT_EQ(permutation.scale(3), 5040 * 840 / 210);
            EXPECT_EQ(permutation.scale(4), 5040 * 840 / 840);
            EXPECT_EQ(permutation.scale(5), 5040 * 840 / 2520);
            EXPECT_EQ(permutation.scale(6), 840);
            EXPECT_EQ(permutation.scale(7), 840);
            EXPECT_EQ(permutation.scale(8), 120);
            EXPECT_EQ(permutation.scale(9), 20);
            EXPECT_EQ(permutation.scale(10), 4);
            EXPECT_EQ(permutation.scale(11), 1);
        }
    }

    namespace {  // Permutation
        class PermutationTest : public ::testing::Test {
        };

        TEST_F(PermutationTest, piecesTI) {
            auto pieces = std::vector<core::PieceType>{
                    core::PieceType::T, core::PieceType::I
            };
            auto permutation = Permutation::create(pieces, 1);

            EXPECT_EQ(permutation.indexSize(), 2);
            EXPECT_EQ(permutation.depth(), 1);

            ASSERT_THAT(permutation.pieces(0), ElementsAre(core::PieceType::T));
            ASSERT_THAT(permutation.pieces(1), ElementsAre(core::PieceType::I));

            EXPECT_EQ(permutation.scale(0), 2);
        }

        TEST_F(PermutationTest, numberifyLJ) {
            auto pieces = std::vector<core::PieceType>{
                    core::PieceType::L, core::PieceType::J
            };
            auto permutation = Permutation::create(pieces, 1);

            EXPECT_EQ(permutation.indexSize(), 2);
            EXPECT_EQ(permutation.depth(), 1);

            for (int number = 0; number < permutation.indexSize(); ++number) {
                auto vector = permutation.pieces(number);
                auto deque = std::deque<core::PieceType>(vector.begin(), vector.end());
                auto queue = std::queue<core::PieceType, std::deque<core::PieceType>>(deque);
                ASSERT_EQ(permutation.numberifyAndPop(queue), number);
            }

            EXPECT_EQ(permutation.scale(0), 2);
        }

        TEST_F(PermutationTest, piecesSZO) {
            auto pieces = std::vector<core::PieceType>{
                    core::PieceType::O, core::PieceType::Z, core::PieceType::S
            };
            auto permutation = Permutation::create(pieces, 2);

            EXPECT_EQ(permutation.indexSize(), 6);
            EXPECT_EQ(permutation.depth(), 2);

            {
                auto vector = std::vector<core::PieceType>{};
                permutation.piecesAndPush(vector, 0);
                ASSERT_THAT(vector, ElementsAre(core::PieceType::O, core::PieceType::Z));
            }

            {
                auto vector = std::vector<core::PieceType>{};
                permutation.piecesAndPush(vector, 5);
                ASSERT_THAT(vector, ElementsAre(core::PieceType::S, core::PieceType::Z));
            }

            EXPECT_EQ(permutation.scale(0), 6);
            EXPECT_EQ(permutation.scale(1), 2);
        }

        TEST_F(PermutationTest, numberifySLOT) {
            auto pieces = std::vector<core::PieceType>{
                    core::PieceType::S, core::PieceType::L, core::PieceType::O, core::PieceType::T
            };
            auto permutation = Permutation::create(pieces, 3);

            EXPECT_EQ(permutation.indexSize(), 4 * 3 * 2);
            EXPECT_EQ(permutation.depth(), 3);

            for (int number = 0; number < permutation.indexSize(); ++number) {
                auto vector = permutation.pieces(number);
                auto deque = std::deque<core::PieceType>(vector.begin(), vector.end());
                auto queue = std::queue<core::PieceType, std::deque<core::PieceType>>(deque);
                ASSERT_EQ(permutation.numberifyAndPop(queue), number);
            }

            EXPECT_EQ(permutation.scale(0), 24);
            EXPECT_EQ(permutation.scale(1), 6);
            EXPECT_EQ(permutation.scale(2), 2);
        }

        TEST_F(PermutationTest, allPieceType) {
            auto permutation = Permutation::create<7>(core::kAllPieceType, 3);

            EXPECT_EQ(permutation.indexSize(), 7 * 6 * 5);
            EXPECT_EQ(permutation.depth(), 3);

            ASSERT_THAT(permutation.pieces(0),
                        ElementsAre(core::PieceType::T, core::PieceType::I, core::PieceType::L));
            ASSERT_THAT(permutation.pieces(1),
                        ElementsAre(core::PieceType::T, core::PieceType::I, core::PieceType::J));
            ASSERT_THAT(permutation.pieces(209),
                        ElementsAre(core::PieceType::O, core::PieceType::Z, core::PieceType::S));

            EXPECT_EQ(permutation.scale(0), 210);
            EXPECT_EQ(permutation.scale(1), 30);
            EXPECT_EQ(permutation.scale(2), 5);
        }
    }
}
