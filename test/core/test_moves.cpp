#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"

namespace core {
    using namespace std::literals::string_literals;

    bool assertMove(std::vector<Move> &moves, const Move &move) {
        auto result = std::find(moves.begin(), moves.end(), move);
        return result != moves.end();
    }

    namespace harddrop {
        class HarddropMoveGeneratorTest : public ::testing::Test {
        };

        TEST_F(HarddropMoveGeneratorTest, case1) {
            auto field = createField(
                    "XXXX____XX"s +
                    "XXXX___XXX"s +
                    "XXXX__XXXX"s +
                    "XXXX___XXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = harddrop::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::T, 4);

                EXPECT_EQ(moves.size(), 6);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 4, 1, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 5, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 5, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 6, 3, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1, true}));
            }

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::S, 4);

                EXPECT_EQ(moves.size(), 3);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 1, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1, true}));
            }
        }

    }

    namespace srs {
        class SRSMoveGeneratorTest : public ::testing::Test {
        };

        TEST_F(SRSMoveGeneratorTest, case1) {
            auto field = createField(
                    "XXXX____XX"s +
                    "XXXX___XXX"s +
                    "XXXX__XXXX"s +
                    "XXXX___XXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = srs::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::T, 4);

                EXPECT_EQ(moves.size(), 7);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 0, false}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 4, 1, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 5, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 5, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 6, 3, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1, true}));
            }

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::S, 4);

                EXPECT_EQ(moves.size(), 3);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 1, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 2, true}));
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1, true}));
            }
        }

        TEST_F(SRSMoveGeneratorTest, case2) {
            auto field = createField(
                    "XXXXXX__XX"s +
                    "XXXXX__XXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = srs::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::S, 2);

                EXPECT_EQ(moves.size(), 1);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 0, false}));
            }
        }

        TEST_F(SRSMoveGeneratorTest, case3) {
            auto field = createField(
                    "XXXX_X__XX"s +
                    "XXXX__XXXX"s +
                    "XXXX___XXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = srs::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::S, 3);

                EXPECT_EQ(moves.size(), 0);
            }
        }

        TEST_F(SRSMoveGeneratorTest, case4) {
            auto field = createField(
                    "_XXXXXXXXX"s +
                    "__XXXXXXXX"s +
                    "X_XXXXXXXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = srs::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::S, 3);

                EXPECT_EQ(moves.size(), 0);
            }
        }

        TEST_F(SRSMoveGeneratorTest, case5) {
            auto field = createField(
                    "XX__XXXXXX"s +
                    "XX___XXXXX"s +
                    "XXX__XXXXX"s +
                    "XXX___XXXX"s +
                    ""
            );

            auto factory = Factory::create();
            auto generator = srs::MoveGenerator(factory);

            {
                auto moves = std::vector<Move>();
                generator.search(moves, field, PieceType::Z, 4);

                EXPECT_EQ(moves.size(), 1);
                EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 3, 1, false}));

                EXPECT_FALSE(assertMove(moves, Move{RotateType::Spawn, 4, 0, false}));
            }
        }
    }

    namespace srs_rotate_end {
        class SRSRotateEndReachableTest : public ::testing::Test {
        };

        TEST_F(SRSRotateEndReachableTest, case1) {
            auto factory = Factory::create();
            auto reachable = srs_rotate_end::Reachable(factory);

            {
                auto field = createField(
                        "XXXX__XXXX"s +
                        "XXXXX__XXX"s +
                        ""
                );

                EXPECT_TRUE(reachable.checks(field, PieceType::Z, RotateType::Spawn, 5, 0, 24));
                EXPECT_TRUE(reachable.checks(field, PieceType::Z, RotateType::Reverse, 5, 1, 24));
            }

            {
                auto field = createField(
                        "XX________"s +
                        "X_________"s +
                        "X_XXXXXXXX"s +
                        "X_XXXXXXXX"s +
                        "X_XXXXXXXX"s +
                        ""
                );

                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Left, 1, 1, 24));
                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Right, 1, 2, 24));
            }

            {
                auto field = createField(
                        "XX________"s +
                        "X_________"s +
                        "X_X_XXXXXX"s +
                        "X_X_XXXXXX"s +
                        "X_XXXXXXXX"s +
                        ""
                );

                EXPECT_FALSE(reachable.checks(field, PieceType::I, RotateType::Left, 1, 1, 24));
                EXPECT_FALSE(reachable.checks(field, PieceType::I, RotateType::Right, 1, 2, 24));
            }

            {
                auto field = createField(
                        "XXX_______"s +
                        "XXX_______"s +
                        "XXX_XXXXXX"s +
                        "XXX____XXX"s +
                        ""
                );

                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Reverse, 5, 0, 24));
                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Spawn, 4, 0, 24));
            }

            {
                auto field = createField(
                        "__________"s +
                        "XXX_______"s +
                        "X____XXXXX"s +
                        "XXX____XXX"s +
                        ""
                );

                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Reverse, 3, 1, 24));
                EXPECT_TRUE(reachable.checks(field, PieceType::I, RotateType::Spawn, 2, 1, 24));

                EXPECT_FALSE(reachable.checks(field, PieceType::I, RotateType::Reverse, 5, 0, 24));
                EXPECT_FALSE(reachable.checks(field, PieceType::I, RotateType::Spawn, 4, 0, 24));
            }

            {
                auto field = createField(
                        "X__XXXXXXX"s +
                        "X___XXXXXX"s +
                        "XX__XXXXXX"s +
                        ""
                );

                EXPECT_TRUE(reachable.checks(field, PieceType::O, RotateType::Spawn, 1, 1, 24));

                EXPECT_FALSE(reachable.checks(field, PieceType::O, RotateType::Spawn, 2, 0, 24));
                EXPECT_FALSE(reachable.checks(field, PieceType::O, RotateType::Right, 2, 1, 24));
                EXPECT_FALSE(reachable.checks(field, PieceType::O, RotateType::Reverse, 3, 1, 24));
                EXPECT_FALSE(reachable.checks(field, PieceType::O, RotateType::Left, 3, 0, 24));
            }
        }
    }
}
