#include "gtest/gtest.h"

#include "sfinder/all_pcs/minos.hpp"

namespace sfinder::all_pcs {
    using namespace std::literals::string_literals;

    class MinosTest : public ::testing::Test {
    };

    TEST_F(MinosTest, g1) {
        auto factory = core::Factory::create();

        {
            int height = 4;

            auto expected = std::vector<int>{
                    0, 1, 40, 127, 218,
                    309, 400, 491, 582, 673,
                    764,
            };

            for (int width = 0; width < expected.size(); ++width) {
                auto minos = std::vector<Mino>{};
                putMinos(factory, width, height, minos);
                EXPECT_EQ(minos.size(), expected[width]);
            }
        }

        {
            int height = 6;

            auto expected = std::vector<int>{
                    0, 15, 205, 515, 831,
                    1147, 1463, 1779, 2095, 2411,
                    2727,
            };

            for (int width = 0; width < expected.size(); ++width) {
                auto minos = std::vector<Mino>{};
                putMinos(factory, width, height, minos);
                EXPECT_EQ(minos.size(), expected[width]);
            }
        }
    }
}
