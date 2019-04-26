#include "gtest/gtest.h"

#include "sample.hpp"
#include "double.hpp"
#include "triple.hpp"

namespace {
    using namespace core;
    
    class SampleTest : public ::testing::Test {
    };

    TEST_F(SampleTest, Sample) {
        int val = Sample().add(5);
        EXPECT_EQ(val, 5);
    }

    TEST_F(SampleTest, Double) {
        int val = DoubleSample().add(1, 2);
        EXPECT_EQ(val, 3);
    }

    TEST_F(SampleTest, Triple) {
        int val = TripleSample().add(1, 2, 3);
        EXPECT_EQ(val, 6);
    }
}
