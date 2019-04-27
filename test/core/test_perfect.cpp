#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/move.hpp"

namespace core {
    class PerfectTest : public ::testing::Test {
    };

    /**

struct Obj {
    int index;
    int holdIndex;
    int line;
    int depth;
};

template<int N>
bool run(
        const Factory &factory,
        Field &field,
        srs::Searcher &searcher,
        std::array<std::vector<Move>, N> &movePool,
        std::array<PieceType, N> pieces,
        Obj obj
) {
    if (N <= obj.depth) {
        return false;
    }

    auto line = obj.line;

    auto moves = movePool[obj.depth];

    if (obj.index < N) {
        auto &current = pieces[obj.index];

        moves.clear();
        searcher.search(moves, factory, field, current, line);

        for (const auto &move : moves) {
            auto &blocks = factory.get(current, move.rotateType);
            auto freeze = Field(field);

            freeze.put(blocks, move.x, move.y);

            int numCleared = freeze.clearLineReturnNum();

            int i = line - numCleared;
            if (i == 0) {
                return true;
            }

            bool result = run<N>(factory, freeze, searcher, movePool, pieces,
                                 Obj{obj.index + 1, obj.holdIndex, i, obj.depth + 1});

            if (result) {
                return true;
            }
        }
    }

    if (0 <= obj.holdIndex && pieces[obj.index] != pieces[obj.holdIndex]) {
        auto &current = pieces[obj.holdIndex];

        moves.clear();
        searcher.search(moves, factory, field, current, line);

        for (const auto &move : moves) {
            auto &blocks = factory.get(current, move.rotateType);
            auto freeze = Field(field);

            freeze.put(blocks, move.x, move.y);
            int numCleared = freeze.clearLineReturnNum();

            int i = line - numCleared;
            if (i == 0) {
                return true;
            }

            bool result = run<N>(factory, freeze, searcher, movePool, pieces,
                                 Obj{obj.index + 1, obj.index, i, obj.depth + 1});

            if (result) {
                return true;
            }
        }
    }

    return false;
}

template<int N>
bool run(
        const Factory &factory,
        srs::Searcher &searcher,
        const Field &field,
        std::array<PieceType, N> pieces,
        int line
) {
    Obj obj{1, 0, line, 0};

    Field freeze = Field(field);

    std::array<std::vector<Move>, N> movePool;
    for (int i = 0; i < movePool.size(); ++i) {
        movePool[i] = std::vector<Move>();
    }

    return run<N>(factory, freeze, searcher, movePool, pieces, obj);
}

TEST_F(PerfectTest, block) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXXXX__XXX"s +
            "XXXX__XXXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::Z, PieceType::S};

    bool result = run<2>(factory, searcher, field, pieces, 2);
    EXPECT_TRUE(result);
}

TEST_F(PerfectTest, block2) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXXXX__XXX"s +
            "XXXX__XXXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::S, PieceType::Z};

    bool result = run<2>(factory, searcher, field, pieces, 2);
    EXPECT_TRUE(result);
}

TEST_F(PerfectTest, block3) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXX____XXX"s +
            "XXXX__XXXX"s +
            "XXXX__XXXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::S, PieceType::O, PieceType::Z};

    bool result = run<3>(factory, searcher, field, pieces, 3);
    EXPECT_TRUE(result);
}

TEST_F(PerfectTest, block4) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXXX____XX"s +
            "XXXX___XXX"s +
            "XXXX__XXXX"s +
            "XXXX___XXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::S, PieceType::I, PieceType::O, PieceType::Z};

    bool result = run<4>(factory, searcher, field, pieces, 4);
    EXPECT_FALSE(result);
}

TEST_F(PerfectTest, block5) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXX_____XX"s +
            "XXX____XXX"s +
            "XXX___XXXX"s +
            "XXX____XXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::I, PieceType::L, PieceType::Z, PieceType::O, PieceType::J};

    bool result = run<5>(factory, searcher, field, pieces, 4);
    EXPECT_FALSE(result);
}

TEST_F(PerfectTest, block6) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "XXX_____XX"s +
            "XXX____XXX"s +
            "XXX___XXXX"s +
            "XXX____XXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::I, PieceType::S, PieceType::L, PieceType::Z, PieceType::O};

    bool result = run<5>(factory, searcher, field, pieces, 4);
    EXPECT_TRUE(result);
}

TEST_F(PerfectTest, block7) {
    using namespace std::literals::string_literals;

    auto field = createField(
            "______XXXX"s +
            "______XXXX"s +
            "______XXXX"s +
            "______XXXX"s +
            ""
    );

    auto factory = Factory::create();
    auto cache = srs::Cache();
    auto searcher = srs::Searcher(cache);

    auto pieces = std::array{PieceType::I, PieceType::L, PieceType::Z, PieceType::O, PieceType::J, PieceType::S};

    bool result = run<6>(factory, searcher, field, pieces, 4);
    EXPECT_TRUE(result);
}
 */
}
