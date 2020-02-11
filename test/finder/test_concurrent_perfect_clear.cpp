#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"
#include "finder/concurrent_perfect_clear.hpp"
#include "finder/thread_pool.hpp"

namespace finder {
    namespace {
        void verify(const core::Factory &factory, const core::Field &field, const finder::Solution &solution) {
            auto freeze = core::Field(field);

            for (const auto &operation : solution) {
                auto &blocks = factory.get(operation.pieceType, operation.rotateType);
                EXPECT_TRUE(freeze.canPut(blocks, operation.x, operation.y));
                freeze.put(blocks, operation.x, operation.y);
                freeze.clearLine();
            }

            EXPECT_EQ(freeze, core::Field{});
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
    }

    using namespace std::literals::string_literals;

    class ConcurrentPerfectClearTest : public ::testing::Test {
    };

    template<class F>
    void check(F &finder) {
        auto field = core::createField(
                "__________"s +
                "__________"s +
                "X_________"s +
                "X_________"s +
                "XXXXXXXXX_"s +
                "XXX_XXXXXX"s +
                ""
        );

        auto maxLine = 6;

        {
            auto pieces = std::vector{
                    core::PieceType::T,

                    core::PieceType::O, core::PieceType::S, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::J, core::PieceType::L,

                    core::PieceType::T, core::PieceType::I, core::PieceType::J, core::PieceType::L,
            };
            auto result = finder.run(field, pieces, maxLine, false);
            show(field, result, maxLine);
        }
    }

    void show(const core::Field &field, const Solution &solution, int height) {
        auto freeze = core::Field(field);
        auto factory = core::Factory::create();
        for (const auto &item : solution) {
            auto blocks = factory.get(item.pieceType, item.rotateType);
            freeze.put(blocks, item.x, item.y);
            std::cout << freeze.toString(height) << std::endl;
            std::cout << std::endl;
            freeze.clearLine();
        }
    }

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(ConcurrentPerfectClearTest, case1) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::S};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::Z};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 1 && piece = 2 && first hold exists
    TEST_F(ConcurrentPerfectClearTest, case2) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "XXXXX__XXX"s +
                "XXXX__XXXX"s +
                ""
        );
        auto maxLine = 2;

        {
            auto pieces = std::vector{core::PieceType::Z, core::PieceType::S};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::T, core::PieceType::Z};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 3 && first hold is empty
    TEST_F(ConcurrentPerfectClearTest, case3) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold is empty
    TEST_F(ConcurrentPerfectClearTest, case4) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::J, core::PieceType::I};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::L, core::PieceType::I, core::PieceType::O};
            auto result = finder.run(field, pieces, maxLine, true);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(ConcurrentPerfectClearTest, case5) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "____XXXXXX"s +
                "___XXXXXXX"s +
                "__XXXXXXXX"s +
                "___XXXXXXX"s +
                ""
        );
        auto maxLine = 4;

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::T, core::PieceType::O};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::O, core::PieceType::J, core::PieceType::S, core::PieceType::T};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::J, core::PieceType::Z, core::PieceType::T};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{core::PieceType::S, core::PieceType::O, core::PieceType::L, core::PieceType::T};
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }
    }

    // depth = 3 && piece = 4 && first hold exists
    TEST_F(ConcurrentPerfectClearTest, case6) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto field = core::createField(
                "XX________"s +
                "XX________"s +
                "XXX______X"s +
                "XXXXXXX__X"s +
                "XXXXXX___X"s +
                "XXXXXXX_XX"s +
                ""
        );
        auto maxLine = 6;

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::I, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_FALSE(!result.empty());
        }

        {
            auto pieces = std::vector{
                    core::PieceType::S, core::PieceType::J, core::PieceType::L, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::I, core::PieceType::T
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::S, core::PieceType::O, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::J, core::PieceType::O, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }

        {
            auto pieces = std::vector{
                    core::PieceType::I, core::PieceType::J, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::O, core::PieceType::S, core::PieceType::L
            };
            auto result = finder.run(field, pieces, maxLine, false);
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }
    }

    TEST_F(ConcurrentPerfectClearTest, longtest1) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

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

            auto result = finder.run(
                    field, pieces, maxDepth, maxLine, false, false, 3, true, SearchTypes::TSpin, false,
                    0b11111111, 2
            );

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

    TEST_F(ConcurrentPerfectClearTest, longtest2) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

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

            auto result = finder.run(
                    field, pieces, maxDepth, maxLine, false, true, 0, true, SearchTypes::TSpin, false,
                    0b11111111, 2
            );

            if (!result.empty()) {
                success += 1;
            }

            auto elapsed = std::chrono::system_clock::now() - start;
            totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        std::cout << totalTime / static_cast<double>(max) << " milli seconds" << std::endl;

        EXPECT_EQ(success, 3248);
    }

    TEST_F(ConcurrentPerfectClearTest, twoLineFollowUp) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(4);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        {
            auto field = core::createField(
                    "XXXX___XXX"s +
                    "XXXX___XXX"s +
                    "XXXX__XXXX"s +
                    ""
            );
            auto maxLine = 3;

            auto pieces = std::vector{
                    core::PieceType::L, core::PieceType::J, core::PieceType::S,
                    core::PieceType::J, core::PieceType::I, core::PieceType::O, core::PieceType::O,
            };
            auto result = finder.run(
                    field, pieces, maxLine, true, SearchTypes::Fast, true, 0, true, true
            );
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);

            EXPECT_EQ(result[0].pieceType, core::PieceType::J);
            EXPECT_EQ(result[1].pieceType, core::PieceType::S);
        }

        {
            auto field = core::createField(
                    "__________"s +
                    "__________"s +
                    ""
            );
            auto maxLine = 2;

            auto pieces = std::vector{
                    core::PieceType::L,
                    core::PieceType::J, core::PieceType::I, core::PieceType::O, core::PieceType::O,
            };
            auto result = finder.run(
                    field, pieces, maxLine, false, SearchTypes::Fast, true, 0, true, true
            );
            EXPECT_TRUE(!result.empty());
            verify(factory, field, result);
        }
    }
}