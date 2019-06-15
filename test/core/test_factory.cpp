#include "gtest/gtest.h"
#include "core/types.hpp"
#include "core/factory.hpp"

namespace core {
    class FactoryTest : public ::testing::Test {
    };

    TEST_F(FactoryTest, getPiece) {
        auto factory = Factory::create();

        {
            const Piece &piece = factory.get(PieceType::T);
            EXPECT_EQ(piece.name, std::string("T"));
        }

        {
            const Piece &piece = factory.get(PieceType::I);
            EXPECT_EQ(piece.name, std::string("I"));
        }

        {
            const Piece &piece = factory.get(PieceType::L);
            EXPECT_EQ(piece.name, std::string("L"));
        }

        {
            const Piece &piece = factory.get(PieceType::J);
            EXPECT_EQ(piece.name, std::string("J"));
        }

        {
            const Piece &piece = factory.get(PieceType::S);
            EXPECT_EQ(piece.name, std::string("S"));
        }

        {
            const Piece &piece = factory.get(PieceType::Z);
            EXPECT_EQ(piece.name, std::string("Z"));
        }

        {
            const Piece &piece = factory.get(PieceType::O);
            EXPECT_EQ(piece.name, std::string("O"));
        }
    }

    TEST_F(FactoryTest, getBlocks) {
        auto factory = Factory::create();

        {
            const Blocks &blocks = factory.get(PieceType::T, RotateType::Spawn);
            EXPECT_EQ(blocks.minX, -1);
            EXPECT_EQ(blocks.maxX, 1);
            EXPECT_EQ(blocks.minY, 0);
            EXPECT_EQ(blocks.maxY, 1);
        }

        {
            const Blocks &blocks = factory.get(PieceType::T, RotateType::Right);
            EXPECT_EQ(blocks.minX, 0);
            EXPECT_EQ(blocks.maxX, 1);
            EXPECT_EQ(blocks.minY, -1);
            EXPECT_EQ(blocks.maxY, 1);
        }

        {
            const Blocks &blocks = factory.get(PieceType::O, RotateType::Reverse);
            EXPECT_EQ(blocks.minX, -1);
            EXPECT_EQ(blocks.maxX, 0);
            EXPECT_EQ(blocks.minY, -1);
            EXPECT_EQ(blocks.maxY, 0);
        }
    }

    TEST_F(FactoryTest, case1) {
        auto factory = Factory::create();

        {
            const Piece &piece = factory.get(PieceType::T);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0001);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b0010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0100);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1000);
        }

        {
            const Piece &piece = factory.get(PieceType::L);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0001);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b0010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0100);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1000);
        }

        {
            const Piece &piece = factory.get(PieceType::J);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0001);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b0010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0100);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1000);
        }

        {
            const Piece &piece = factory.get(PieceType::I);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b1010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1010);
        }

        {
            const Piece &piece = factory.get(PieceType::S);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b1010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1010);
        }

        {
            const Piece &piece = factory.get(PieceType::Z);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b1010);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b0101);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1010);
        }

        {
            const Piece &piece = factory.get(PieceType::O);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Spawn], 0b1111);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Right], 0b1111);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Reverse], 0b1111);
            EXPECT_EQ(piece.sameShapeRotates[RotateType::Left], 0b1111);
        }
    }
}
