#include "gtest/gtest.h"

#include <chrono>

#include "core/field.hpp"
#include "core/moves.hpp"
#include "finder/perfect_clear.hpp"

namespace finder {
    using namespace std::literals::string_literals;

    class PerfectClearTest : public ::testing::Test {
    };

    TEST_F(PerfectClearTest, getTSpinShape) {
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

    TEST_F(PerfectClearTest, getAttackIfTSpin) {
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
            EXPECT_EQ(getAttackIfTSpin(reachable, factory, field, core::PieceType::T, move, 2, true), 0);
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

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(PerfectClearTest, case1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxDepth = 1;
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::S};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 1 && piece = 2 && first hold exists
    TEST_F(PerfectClearTest, case2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxDepth = 1;
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::Z, core::PieceType::S};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::T, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 3 && first hold is empty
    TEST_F(PerfectClearTest, case3) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold is empty
    TEST_F(PerfectClearTest, case4) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectClearTest, case5) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxDepth = 3;
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::T, core::PieceType::O};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::O, core::PieceType::J, core::PieceType::S, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::Z, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::L, core::PieceType::T};
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(PerfectClearTest, case6) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        auto maxDepth = 7;
        auto maxLine = 6;

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::I, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::S, core::PieceType::J, core::PieceType::L, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::I, core::PieceType::T
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
            EXPECT_TRUE(!result.empty());
        }
    }

    template<int N>
    std::array<core::PieceType, N> toPieces(int value) {
        int arr[N];

        for (int index = N - 1; 0 <= index; --index) {
            int scale = 7 - index;
            arr[index] = value % scale;
            value /= scale;
        }

        for (int select = N - 2; 0 <= select; --select) {
            for (int adjust = select + 1; adjust < N; ++adjust) {
                if (arr[select] <= arr[adjust]) {
                    arr[adjust] += 1;
                }
            }
        }

        std::array<core::PieceType, N> pieces = {};
        for (int index = 0; index < N; ++index) {
            pieces[index] = static_cast<core::PieceType>(arr[index]);
        }

        return pieces;
    }

    TEST_F(PerfectClearTest, longtest1) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        const int maxDepth = 7;
        const int maxLine = 6;

        int success = 0;
        long long int totalTime = 0L;
        int max = 5040;
        for (int value = 0; value < max; ++value) {
            auto arr = toPieces<maxDepth>(value);
            auto pieces = std::vector(arr.begin(), arr.end());

            auto start = std::chrono::system_clock::now();

            auto result = finder.run(field, pieces, maxDepth, maxLine, false, false, 3);

            // Failed: 975, 2295
            if (!result.empty()) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 5038);
    }

    TEST_F(PerfectClearTest, longtest2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = perfect_clear::Finder<core::srs::MoveGenerator>(factory, moveGenerator);

        auto field = core::createField(
                "__________"s +
                "_XX_______"s +
                "XXXXX____X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        const int maxDepth = 7;
        const int maxLine = 6;

        int success = 0;
        long long int totalTime = 0L;
        int max = 5040;
        for (int value = 0; value < max; ++value) {
            auto arr = toPieces<maxDepth>(value);
            auto pieces = std::vector(arr.begin(), arr.end());

            auto start = std::chrono::system_clock::now();

            auto result = finder.run(field, pieces, maxDepth, maxLine, false, true, 0);

            if (!result.empty()) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 3248);
    }
}
