#include "gtest/gtest.h"

#include "sfinder/marker.hpp"

namespace sfinder {
    namespace {
        class MarkerTest : public ::testing::Test {
        };

        TEST_F(MarkerTest, succeed) {
            auto marker = Marker::create(5040);

            for (int index = 0; index < 5040; ++index) {
                ASSERT_EQ(marker.calculated(index), false);
                ASSERT_EQ(marker.succeed(index), false);

                marker.set(index, true);

                ASSERT_EQ(marker.calculated(index), true);
                EXPECT_EQ(marker.succeed(index), true);
            }

            for (int index = 0; index < 5040; ++index) {
                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), true);

                marker.set(index, true);

                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), true);
            }
        }

        TEST_F(MarkerTest, failed) {
            auto marker = Marker::create(5040);

            for (int index = 0; index < 5040; ++index) {
                ASSERT_EQ(marker.calculated(index), false);
                ASSERT_EQ(marker.succeed(index), false);

                marker.set(index, false);

                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), false);
            }

            for (int index = 0; index < 5040; ++index) {
                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), false);

                marker.set(index, true);

                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), true);
            }

            for (int index = 0; index < 5040; ++index) {
                ASSERT_EQ(marker.calculated(index), true);
                ASSERT_EQ(marker.succeed(index), true);

                marker.set(index, false);

                ASSERT_EQ(marker.calculated(index), true);

                // Once set success, it will not be set failed
                ASSERT_EQ(marker.succeed(index), true);
            }
        }
    }

    namespace {
        class BitsTest : public ::testing::Test {
        };

        TEST_F(BitsTest, checks) {
            auto bits = Bits::create(5040);

            for (int index = 0; index < 5040; ++index) {
                ASSERT_FALSE(bits.checks(index));

                bits.set(index, false);

                ASSERT_FALSE(bits.checks(index));
            }

            for (int index = 0; index < 5040; ++index) {
                ASSERT_FALSE(bits.checks(index));

                bits.set(index, true);

                ASSERT_TRUE(bits.checks(index));
            }

            for (int index = 0; index < 5040; ++index) {
                ASSERT_TRUE(bits.checks(index));

                bits.set(index, false);

                ASSERT_FALSE(bits.checks(index));
            }
        }
    }
}
