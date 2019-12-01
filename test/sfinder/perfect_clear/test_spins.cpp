#include "gtest/gtest.h"

#include "core/field.hpp"
#include "sfinder/perfect_clear/full_finder.hpp"

namespace sfinder::perfect_clear {
    using namespace std::literals::string_literals;

    class SpinsTest : public ::testing::Test {
    };

    TEST_F(SpinsTest, getTSpinShape) {
        {
            auto field = core::createField(
                    "X_________"s +
                    "__________"s +
                    "X_XXXXXXX_"s +
                    ""
            );

            EXPECT_EQ(getTSpinShape(field, 1, 1, core::RotateType::Reverse), TSpinShapes::RegularShape);
            EXPECT_EQ(getTSpinShape(field, 1, 1, core::RotateType::Left), TSpinShapes::RegularShape);
            EXPECT_EQ(getTSpinShape(field, 1, 1, core::RotateType::Right), TSpinShapes::MiniOrTSTShape);
            EXPECT_EQ(getTSpinShape(field, 1, 1, core::RotateType::Spawn), TSpinShapes::MiniOrTSTShape);

            EXPECT_EQ(getTSpinShape(field, 9, 1, core::RotateType::Left), TSpinShapes::MiniOrTSTShape);
        }

        {
            auto field = core::createField(
                    "__________"s +
                    "______XX__"s +
                    "_XXXXXX___"s +
                    ""
            );

            EXPECT_EQ(getTSpinShape(field, 8, 0, core::RotateType::Spawn), TSpinShapes::MiniOrTSTShape);

            EXPECT_EQ(getTSpinShape(field, 0, 1, core::RotateType::Right), TSpinShapes::MiniOrTSTShape);
        }

        {
            auto field = core::createField(
                    "XX______XX"s +
                    "X________X"s +
                    "X_XXXXXX_X"s +
                    "X__XXXX__X"s +
                    "X_XXXXXX_X"s +
                    ""
            );

            EXPECT_EQ(getTSpinShape(field, 1, 1, core::RotateType::Right), TSpinShapes::MiniOrTSTShape);

            EXPECT_EQ(getTSpinShape(field, 8, 1, core::RotateType::Left), TSpinShapes::MiniOrTSTShape);
        }
    }

    TEST_F(SpinsTest, getAttackIfTSpin) {
        auto factory = core::Factory::create();
        auto reachable = core::srs_rotate_end::Reachable(factory);

        {
            auto field = core::createField(
                    "XXX__XXXXX"s +
                    "XX__XXXXXX"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Spawn, 3, 0, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::S, move, 2, false), 0);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::S, move, 2, true), 0);
        }

        {
            auto field = core::createField(
                    "XX________"s +
                    "X___XXXXXX"s +
                    "XX_XXXXXXX"s +
                    ""
            );

            {
                auto move = core::Move{core::RotateType::Reverse, 2, 1, false};
                EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, false), 4);
                EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, true), 5);
            }
            {
                auto move = core::Move{core::RotateType::Left, 2, 1, false};
                EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 1, false), 2);
                EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 1, true), 3);
            }
            {
                auto move = core::Move{core::RotateType::Spawn, 2, 1, false};
                EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 1, false), 0);
            }
        }

        {
            auto field = core::createField(
                    "__________"s +
                    "__________"s +
                    "XXXXXXXXX_"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 9, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 1, false), 0);
        }

        {
            auto field = core::createField(
                    "__________"s +
                    "______XX__"s +
                    "XXXXXXX___"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Spawn, 8, 0, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 1, false), 0);
        }

        {
// TST
            auto field = core::createField(
                    "________XX"s +
                    "_________X"s +
                    "XXXXXXXX_X"s +
                    "XXXXXXX__X"s +
                    "XXXXXXXX_X"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 8, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 3, false), 6);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 3, true), 7);
        }

        {
// NEO
            auto field = core::createField(
                    "_______XXX"s +
                    "_________X"s +
                    "________XX"s +
                    "XXXXXX__XX"s +
                    "XXXXXXX_XX"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 7, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, false), 0);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, true), 1);
        }

        {
// FIN
            auto field = core::createField(
                    "______XXXX"s +
                    "________XX"s +
                    "________XX"s +
                    "XXXXXX__XX"s +
                    "XXXXXXX_XX"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 7, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, false), 4);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, true), 5);
        }

        {
// ISO
            auto field = core::createField(
                    "XXXX______"s +
                    "X_________"s +
                    "XX________"s +
                    "X__XXXXXXX"s +
                    "XX_XXXXXXX"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 2, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, false), 4);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, true), 5);
        }

        {
            auto field = core::createField(
                    "_________X"s +
                    "_________X"s +
                    "XXXXXXXX_X"s +
                    "XXXXXXX__X"s +
                    "XXXXXXXX_X"s +
                    ""
            );

            auto move = core::Move{core::RotateType::Left, 8, 1, false};
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 3, false), 0);
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 3, true), 0);
        }
    }

    TEST_F(SpinsTest, getAttackIfAllSpins) {
        auto factory = core::Factory::create();

        {
            auto field = core::createField(""s +
                                           "XXX__XXXXX"s +
                                           "XX__XXXXXX"s +
                                           ""
            );

            auto move = core::Move{core::RotateType::Spawn, 3, 0, false};
            int numCleared = 2;

            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::S, move, numCleared, false), 4);
            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::S, move, numCleared, true), 5);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::S, move, numCleared, false), 4);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::S, move, numCleared, true), 5);
        }

        {
            auto field = core::createField(""s +
                                           "XXX_______"s +
                                           "XX__XXXXXX"s +
                                           ""
            );

            auto move = core::Move{core::RotateType::Spawn, 3, 0, false};
            int numCleared = 1;

            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::S, move, numCleared, false), 2);
            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::S, move, numCleared, true), 3);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::S, move, numCleared, false), 0);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::S, move, numCleared, true), 1);
        }

        {
            auto field = core::createField(""s +
                                           "_XXXXXXXXX"s +
                                           "_XXXXXXXXX"s +
                                           "_XXXXXXXXX"s +
                                           "____XXXXXX"s +
                                           ""
            );

            auto move = core::Move{core::RotateType::Reverse, 2, 0, false};
            int numCleared = 1;

            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::I, move, numCleared, false), 2);
            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::I, move, numCleared, true), 3);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::I, move, numCleared, false), 2);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::I, move, numCleared, true), 3);
        }

        {
            auto field = core::createField(""s +
                                           "X__XXXXXXX"s +
                                           "X__XXXXXXX"s +
                                           "___XXXXXXX"s +
                                           "X__XXXXXXX"s +
                                           ""
            );

            auto move = core::Move{core::RotateType::Reverse, 1, 1, false};
            int numCleared = 1;

            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::J, move, numCleared, false), 2);
            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::J, move, numCleared, true), 3);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::J, move, numCleared, false), 0);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::J, move, numCleared, true), 1);
        }

        {
            auto field = core::createField(""s +
                                           "XXXXXXXX__"s +
                                           "XXXXXXXX__"s +
                                           "XXXXXXX___"s +
                                           "XXXXXXX_XX"s +
                                           ""
            );

            auto move = core::Move{core::RotateType::Reverse, 8, 1, false};
            int numCleared = 1;

            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::L, move, numCleared, false), 2);
            EXPECT_EQ(getAttackIfAllSpins<true>(factory, field, core::PieceType::L, move, numCleared, true), 3);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::L, move, numCleared, false), 0);
            EXPECT_EQ(getAttackIfAllSpins<false>(factory, field, core::PieceType::L, move, numCleared, true), 1);
        }
    }
}