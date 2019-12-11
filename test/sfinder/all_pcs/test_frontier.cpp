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

        auto nodes = Nodes(14, 13);
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

        auto nodes = Nodes(18, 19);
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

        auto nodes = Nodes(23, 25);
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

        auto nodes = Nodes(29, 31);
        auto frontier = Frontier(minos, nodes, width, height);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(10x10)=" << elapsed << std::endl;

        EXPECT_EQ(nodes.indexSerial(), 305405811);
        EXPECT_EQ(nodes.itemNodeSerial(), 1778153314);
    }

    TEST_F(FrontierTest, ag1) {
        int width = 7;
        int height = 4;
        int maxDepth = (width * height) / 4;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(14, 13);
        auto frontier = Frontier(minos, nodes, width, height);

        auto usingLineEachY = std::vector<LineBit>(minos.size() * height);
        for (int minoIndex = 0; minoIndex < minos.size(); ++minoIndex) {
            auto &mino = minos[minoIndex];
            int headIndex = minoIndex * height;
            for (int iy = 0; iy < height; ++iy) {
                if (0 < (mino.deletedLine & (1U << iy))) {
                    usingLineEachY[headIndex + iy] = mino.usingLine;
                }
            }
        }

        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto builder = Builder(factory, minos, width, height, maxDepth, moveGenerator);
        auto aggregator = Aggregator(factory, minos, height, maxDepth, usingLineEachY, nodes, builder);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        aggregator.aggregate();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(" << width << "x" << height << ")=" << elapsed << std::endl;

        std::cout << "[" << aggregator.sum << "]" << std::endl;

        // srs: 23737025
        // deep: 29926718
        // deep & air-lock: 30011878 <- my result 4963ms
    }

    TEST_F(FrontierTest, ag2) {
        int width = 2;
        int height = 6;
        int maxDepth = (width * height) / 4;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(18, 19);
        auto frontier = Frontier(minos, nodes, width, height);

        auto usingLineEachY = std::vector<LineBit>(minos.size() * height);
        for (int minoIndex = 0; minoIndex < minos.size(); ++minoIndex) {
            auto &mino = minos[minoIndex];
            int headIndex = minoIndex * height;
            for (int iy = 0; iy < height; ++iy) {
                if (0 < (mino.deletedLine & (1U << iy))) {
                    usingLineEachY[headIndex + iy] = mino.usingLine;
                }
            }
        }

        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto builder = Builder(factory, minos, width, height, maxDepth, moveGenerator);
        auto aggregator = Aggregator(factory, minos, height, maxDepth, usingLineEachY, nodes, builder);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        aggregator.aggregate();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(" << width << "x" << height << ")=" << elapsed << std::endl;

        std::cout << "[" << aggregator.sum << "]" << std::endl;

        // srs: ???
        // deep: ???
        // deep & air-lock: ??? <- my result
    }

    TEST_F(FrontierTest, ag3) {
        int width = 5;
        int height = 8;
        int maxDepth = (width * height) / 4;

        auto factory = core::Factory::create();
        auto minos = std::vector<Mino>{};
        putMinos(factory, width, height, minos);

        auto nodes = Nodes(23, 25);
        auto frontier = Frontier(minos, nodes, width, height);

        auto usingLineEachY = std::vector<LineBit>(minos.size() * height);
        for (int minoIndex = 0; minoIndex < minos.size(); ++minoIndex) {
            auto &mino = minos[minoIndex];
            int headIndex = minoIndex * height;
            for (int iy = 0; iy < height; ++iy) {
                if (0 < (mino.deletedLine & (1U << iy))) {
                    usingLineEachY[headIndex + iy] = mino.usingLine;
                }
            }
        }

        auto moveGenerator = core::srs::MoveGenerator(factory);
        auto builder = Builder(factory, minos, width, height, maxDepth, moveGenerator);
        auto aggregator = Aggregator(factory, minos, height, maxDepth, usingLineEachY, nodes, builder);

        auto start = std::chrono::system_clock::now();
        frontier.build();
        aggregator.aggregate();
        auto end = std::chrono::system_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "elapsed(" << width << "x" << height << ")=" << elapsed << std::endl;

        std::cout << "[" << aggregator.sum << "]" << std::endl;

        // srs: ???
        // deep: ???
        // deep & air-lock: ??? <- my result
    }
}
