#include "gtest/gtest.h"

#include "nn/main.cpp"

namespace nn {
    using namespace std::literals::string_literals;

    class NNTest : public ::testing::Test {
    };

    TEST_F(NNTest, case1) {
       main();
    }
}
