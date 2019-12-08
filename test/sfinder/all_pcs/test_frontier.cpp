#include "gtest/gtest.h"

#include <chrono>

#include "sfinder/all_pcs/types.hpp"
#include "sfinder/all_pcs/minos.hpp"
#include "sfinder/all_pcs/frontier.hpp"

namespace sfinder::all_pcs {
    using namespace std::literals::string_literals;

    class FrontierTest : public ::testing::Test {
    };

    TEST_F(FrontierTest, case10x4) {
        int width = 10;
        int height = 4;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(1U << 13U, 1U << 13U);
        auto frontier = Frontier(minos, nodes, width, height);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(10x4)=" << elapsed << std::endl;

        EXPECT_EQ(nodes.indexSerial(), 4168);
        EXPECT_EQ(nodes.itemNodeSerial(), 5385);
    }

    TEST_F(FrontierTest, case10x6) {
        int width = 10;
        int height = 6;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(1U << 18U, 1U << 19U);
        auto frontier = Frontier(minos, nodes, width, height);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(10x6)=" << elapsed << std::endl;

        EXPECT_EQ(nodes.indexSerial(), 178069);
        EXPECT_EQ(nodes.itemNodeSerial(), 412515);
    }

    TEST_F(FrontierTest, case10x8) {
        int width = 10;
        int height = 8;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(1U << 23U, 1U << 25U);
        auto frontier = Frontier(minos, nodes, width, height);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(10x8)=" << elapsed << std::endl;

        EXPECT_EQ(nodes.indexSerial(), 7471316);
        EXPECT_EQ(nodes.itemNodeSerial(), 28500020);
    }

    TEST_F(FrontierTest, case10x10) {
        int width = 10;
        int height = 10;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(1U << 29U, 1U << 31U);
        auto frontier = Frontier(minos, nodes, width, height);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(10x10)=" << elapsed << std::endl;

        EXPECT_EQ(nodes.indexSerial(), 305405811);
        EXPECT_EQ(nodes.itemNodeSerial(), 1778153314);
    }
}
