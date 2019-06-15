#include "gtest/gtest.h"
#include "core/field.hpp"
#include "core/factory.hpp"

namespace core {
    using namespace std::literals::string_literals;

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

    TEST_F(FieldTest, canReachOnHarddrop1) {
        auto field = createField(
                std::string("_________X") +
                std::string("__________")
        );

        auto factory = Factory::create();

        {
            auto result = field.canReachOnHarddrop(factory.get(PieceType::T, RotateType::Spawn), 1, 0);
            EXPECT_TRUE(result);
        }

        {
            auto result = field.canReachOnHarddrop(factory.get(PieceType::T, RotateType::Spawn), 8, 0);
            EXPECT_FALSE(result);
        }
    }

    TEST_F(FieldTest, canReachOnHarddrop2) {
        auto field = createField(
                "_XXXXXXXXX"s +
                "__XXXXXXXX"s +
                "X_XXXXXXXX"s +
                ""
        );

        auto factory = Factory::create();

        {
            auto result = field.canReachOnHarddrop(factory.get(PieceType::T, RotateType::Right), 0, 1);
            EXPECT_FALSE(result);
        }
    }

    TEST_F(FieldTest, canPut) {
        auto field = createField(
                std::string("______X___") +
                std::string("_____X____") +
                std::string("____X_____") +
                std::string("___X______") +
                std::string("__X_______") +
                std::string("_X________") +
                std::string("X_________")
        );
        auto factory = Factory::create();

        const auto &blocks = factory.get(PieceType::S, RotateType::Spawn);
        EXPECT_TRUE(field.canPut(blocks, 8, 0));
        EXPECT_TRUE(field.canPut(blocks, 8, 1));
        EXPECT_TRUE(field.canPut(blocks, 8, 2));
        EXPECT_TRUE(field.canPut(blocks, 8, 3));
        EXPECT_TRUE(field.canPut(blocks, 8, 4));
        EXPECT_TRUE(field.canPut(blocks, 8, 5));
        EXPECT_TRUE(field.canPut(blocks, 8, 6));

        EXPECT_FALSE(field.canPut(blocks, 1, 0));
        EXPECT_FALSE(field.canPut(blocks, 2, 1));
        EXPECT_FALSE(field.canPut(blocks, 3, 2));
        EXPECT_FALSE(field.canPut(blocks, 4, 3));
        EXPECT_FALSE(field.canPut(blocks, 5, 4));
        EXPECT_FALSE(field.canPut(blocks, 6, 5));
    }
}
