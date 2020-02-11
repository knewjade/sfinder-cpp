#include "gtest/gtest.h"
#include "core/bits.hpp"

namespace core {
    using namespace std::literals::string_literals;

    class BitsTest : public ::testing::Test {
    };

    TEST_F(BitsTest, mostSignificantDigit) {
        EXPECT_EQ(mostSignificantDigit(0b0ULL), 0);
        EXPECT_EQ(mostSignificantDigit(0b1ULL), 1);
        EXPECT_EQ(mostSignificantDigit(0b1ULL << 1U), 2);
        EXPECT_EQ(mostSignificantDigit(0b1ULL << 2U), 3);
        EXPECT_EQ(mostSignificantDigit(0b1ULL << 63U), 64);
    }
}
