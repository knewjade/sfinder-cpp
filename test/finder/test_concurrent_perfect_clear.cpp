#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"
#include "finder/concurrent_perfect_clear.hpp"
#include "finder/thread_pool.hpp"

namespace finder {
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

        auto maxDepth = 10;
        auto maxLine = 6;

        {
            auto pieces = std::vector{
                    core::PieceType::T,

                    core::PieceType::O, core::PieceType::S, core::PieceType::T, core::PieceType::Z,
                    core::PieceType::J, core::PieceType::L,

                    core::PieceType::T, core::PieceType::I, core::PieceType::J, core::PieceType::L,
            };
            auto result = finder.run(field, pieces, maxDepth, maxLine, false);
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
        check(finder);
    }

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(ConcurrentPerfectClearTest, case1abort) {
        auto factory = core::Factory::create();
        auto threadPool = ThreadPool(6);
        auto finder = ConcurrentPerfectClearFinder<core::srs::MoveGenerator>(factory, threadPool);

        auto thread = std::thread([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "abort" << std::endl;
            finder.abort();
        });

        check(finder);

        thread.join();
    }

    // depth = 1 && piece = 1 && first hold is empty
    TEST_F(ConcurrentPerfectClearTest, case1_2) {
        auto factory = core::Factory::create();
        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto finder = PerfectClearFinder<>(factory, moveGenerator);
        check(finder);
    }
}