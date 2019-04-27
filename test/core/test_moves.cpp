#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"

namespace core {
    using namespace std::literals::string_literals;

    class MovesTest : public ::testing::Test {
    };

    bool assertMove(std::vector<Move> &moves, const Move &move) {
        auto result = std::find_if(moves.begin(), moves.end(), [&](Move it) {
            return move.rotateType == it.rotateType && move.x == it.x && move.y == it.y;
        });
        return result != moves.end();
    }

    TEST_F(MovesTest, case1) {
        auto field = createField(
                "XXXX____XX"s +
                "XXXX___XXX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::T, 4);

            EXPECT_EQ(moves.size(), 7);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 0}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 4, 1}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 5, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 5, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 6, 3}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1}));
        }

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 4);

            EXPECT_EQ(moves.size(), 3);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 1}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1}));
        }
    }

    TEST_F(MovesTest, case2) {
        auto field = createField(
                "XXXXXX__XX"s +
                "XXXXX__XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 2);

            EXPECT_EQ(moves.size(), 1);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 0}));
        }
    }

    TEST_F(MovesTest, case3) {
        auto field = createField(
                "XXXX_X__XX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 3);

            EXPECT_EQ(moves.size(), 0);
        }
    }
}
