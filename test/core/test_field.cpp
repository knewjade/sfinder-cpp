#include "gtest/gtest.h"
#include "core/field.hpp"

namespace core {
    class FieldTest : public ::testing::Test {
    };

    TEST_F(FieldTest, block) {
        auto field = createField(
                std::string("X_________")
        );

        EXPECT_EQ(field.isEmpty(0, 0), false);
        EXPECT_EQ(field.isEmpty(1, 0), true);

        field.setBlock(1, 0);

        EXPECT_EQ(field.isEmpty(0, 0), false);
        EXPECT_EQ(field.isEmpty(1, 0), false);

        for (int x = 0; x < 10; ++x) {
            field.setBlock(x, 0);
        }

        std::cout << field.toString(8) << std::endl;
    }

    TEST_F(FieldTest, equals) {
        auto field1 = createField(
                std::string("X_________")
        );

        auto field2 = createField(
                std::string("__________")
        );
        field2.setBlock(0, 0);

        EXPECT_TRUE(field1 == field2);
    }
}
