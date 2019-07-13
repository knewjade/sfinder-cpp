#include "gtest/gtest.h"

#include "finder/marker.hpp"

namespace finder {
    using namespace std::literals::string_literals;

    class MarkerTest : public ::testing::Test {
    };

    TEST_F(MarkerTest, marker) {
        auto marker = Marker::create(5040);

        {
            int index = 0;

            EXPECT_EQ(marker.calculated(index), false);
            EXPECT_EQ(marker.succeed(index), false);
            EXPECT_EQ(marker.failed(index), false);

            marker.set(index, true);

            EXPECT_EQ(marker.calculated(index), true);
            EXPECT_EQ(marker.succeed(index), true);
            EXPECT_EQ(marker.failed(index), false);
        }

        {
            int index = 1;

            EXPECT_EQ(marker.calculated(index), false);
            EXPECT_EQ(marker.succeed(index), false);
            EXPECT_EQ(marker.failed(index), false);

            marker.set(index, false);

            EXPECT_EQ(marker.calculated(index), true);
            EXPECT_EQ(marker.succeed(index), false);
            EXPECT_EQ(marker.failed(index), true);
        }
    }
}
