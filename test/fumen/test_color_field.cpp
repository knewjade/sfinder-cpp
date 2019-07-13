#include "gtest/gtest.h"

#include <vector>

#include "core/factory.hpp"
#include "fumen/color_field.hpp"
#include "fumen/parser.hpp"

namespace fumen {
    using namespace std::literals::string_literals;

    class ColorFieldTest : public ::testing::Test {
    };

    TEST_F(ColorFieldTest, parseToColorType) {
        auto converter = ColorConverter::create();

        EXPECT_EQ(converter.parseToColorType(core::PieceType::T), ColorType::T);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::I), ColorType::I);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::O), ColorType::O);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::L), ColorType::L);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::J), ColorType::J);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::S), ColorType::S);
        EXPECT_EQ(converter.parseToColorType(core::PieceType::Z), ColorType::Z);
    }

    TEST_F(ColorFieldTest, case1) {
        auto factory = core::Factory::create();
        auto converter = ColorConverter::create();
        auto parser = Parser(factory, converter);

        auto field = ColorField(24);
        field.setBlock(ColorType::Gray, 0, 0);

        auto elements = std::vector<Element>{
                Element{ColorType::O, core::RotateType::Spawn, 6, 0, true, field, "hello"},
        };

        EXPECT_EQ(parser.encode(elements), "bhA8SeTMYFAooMDEPBAAA"s);
    }

    TEST_F(ColorFieldTest, case1_2) {
        auto factory = core::Factory::create();
        auto converter = ColorConverter::create();
        auto parser = Parser(factory, converter);

        auto field = ColorField(24);
        field.setBlock(ColorType::Gray, 0, 0);

        auto elements = std::vector<Element>{
                Element{ColorType::O, core::RotateType::Spawn, 6, 0, true, field, "hello"},
                Element{ColorType::O, core::RotateType::Spawn, 8, 0, true, field, "world"},
        };

        EXPECT_EQ(parser.encode(elements), "bhA8SeTMYFAooMDEPBAAAXhxSHexSLeTtQFA3XaDEE?BAAA"s);
    }

    TEST_F(ColorFieldTest, case2) {
        auto factory = core::Factory::create();
        auto converter = ColorConverter::create();
        auto parser = Parser(factory, converter);

        auto elements = std::vector<Element>{
                Element{ColorType::J, core::RotateType::Spawn, 8, 0},
                Element{ColorType::L, core::RotateType::Spawn, 4, 0},
                Element{ColorType::I, core::RotateType::Left, 6, 1},
                Element{ColorType::S, core::RotateType::Spawn, 4, 1},
                Element{ColorType::Z, core::RotateType::Spawn, 8, 1},
                Element{ColorType::O, core::RotateType::Spawn, 0, 0},
                Element{ColorType::T, core::RotateType::Spawn, 4, 3},
        };

        EXPECT_EQ(parser.encode(elements), "vhGWSJSwBZnBXmBUoBTpBVhB"s);
    }
}
