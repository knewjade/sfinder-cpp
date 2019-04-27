#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/move.hpp"

namespace core {
    class MoveTest : public ::testing::Test {
    };

    TEST_F(MoveTest, block) {
        using namespace std::literals::string_literals;

        auto field = createField(
                "XXXX____XX"s +
                "XXXX___XXX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        std::cout << field.toString(5) << std::endl;

        auto factory = Factory::create();
        auto cache = srs::Cache();
        auto searcher = srs::Searcher(cache);

        {
            auto moves = std::vector<Move>();
            searcher.search(moves, factory, field, PieceType::T, 4);

            for (const auto &move : moves) {
                std::cout << move.rotateType << "," << move.x << "," << move.y << std::endl;
            }

            EXPECT_EQ(moves.size(), 7);
        }

        std::cout << "===========" << std::endl;

        {
            auto moves = std::vector<Move>();
            searcher.search(moves, factory, field, PieceType::S, 4);

            for (const auto &move : moves) {
                std::cout << move.rotateType << "," << move.x << "," << move.y << std::endl;
            }

            EXPECT_EQ(moves.size(), 3);
        }
    }

    TEST_F(MoveTest, block2) {
        using namespace std::literals::string_literals;

        auto field = createField(
                "XXXXXX__XX"s +
                "XXXXX__XXX"s +
                ""
        );

        std::cout << field.toString(3) << std::endl;

        auto factory = Factory::create();
        auto cache = srs::Cache();
        auto searcher = srs::Searcher(cache);

        {
            auto moves = std::vector<Move>();
            searcher.search(moves, factory, field, PieceType::S, 2);

            for (const auto &move : moves) {
                std::cout << move.rotateType << "," << move.x << "," << move.y << std::endl;
            }

            EXPECT_EQ(moves.size(), 1);
        }
    }

    TEST_F(MoveTest, block3) {
        using namespace std::literals::string_literals;

        auto field = createField(
                "XXXX_X__XX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        std::cout << field.toString(4) << std::endl;

        auto factory = Factory::create();
        auto cache = srs::Cache();
        auto searcher = srs::Searcher(cache);

        {
            auto moves = std::vector<Move>();
            searcher.search(moves, factory, field, PieceType::S, 3);

            for (const auto &move : moves) {
                std::cout << move.rotateType << "," << move.x << "," << move.y << std::endl;
            }

            EXPECT_EQ(moves.size(), 0);
        }
    }
}
