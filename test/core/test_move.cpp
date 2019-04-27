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

        auto moves = std::vector<Move>();
        auto factory = Factory::create();
        harddrop::search(moves, factory, field, PieceType::T, 4);

        for (const auto &move : moves) {
            std::cout << move.rotateType << "," << move.x << "," << move.y << std::endl;
        }

//        EXPECT_EQ(field.isEmpty(0, 0), false);
    }
}
