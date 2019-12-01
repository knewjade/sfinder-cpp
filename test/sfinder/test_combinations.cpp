#include "gtest/gtest.h"

#include <vector>
#include <set>

#include "sfinder/combinations.hpp"

namespace sfinder {
    using namespace std::literals::string_literals;
    using namespace ::testing;

    class CombinationsTest : public ::testing::Test {
    };

    int verify(int n, int pop) {
        auto container = std::vector<int>(n);
        for (int i = 0; i < n; ++i) {
            container[i] = i;
        }

        auto hashSet = std::set<long long>{};

        combination(container, pop, [&](const std::vector<int> &vector) {
            EXPECT_EQ(vector.size(), pop);

            auto copied = std::vector(vector.cbegin(), vector.cend());
            std::sort(copied.begin(), copied.end());

            long long hash = 0;
            for (const auto &v : copied) {
                hash *= n;
                hash += v;
            }

            auto result = hashSet.insert(hash);
            EXPECT_TRUE(result.second);
        });

        return hashSet.size();
    }

    TEST_F(CombinationsTest, duplicate) {
        EXPECT_EQ(verify(1, 1), 1);

        EXPECT_EQ(verify(5, 3), 10);

        EXPECT_EQ(verify(10, 1), 10);
        EXPECT_EQ(verify(10, 2), 45);
        EXPECT_EQ(verify(10, 3), 120);
        EXPECT_EQ(verify(10, 4), 210);
        EXPECT_EQ(verify(10, 5), 252);
        EXPECT_EQ(verify(10, 6), 210);
        EXPECT_EQ(verify(10, 7), 120);
        EXPECT_EQ(verify(10, 8), 45);
        EXPECT_EQ(verify(10, 9), 10);
        EXPECT_EQ(verify(10, 10), 1);

        EXPECT_EQ(verify(20, 10), 184756);
    }

    int count(int n, int pop) {
        auto container = std::vector<int>(n);
        for (int i = 0; i < n; ++i) {
            container[i] = i;
        }

        int count = 0;
        combination(container, pop, [&](const std::vector<int> &vector) {
            count++;
        });
        return count;
    }

    TEST_F(CombinationsTest, count) {
        EXPECT_EQ(count(20, 10), 184756);

        EXPECT_EQ(count(30, 10), 30045015);

        EXPECT_EQ(count(100, 99), 100);
    }
}
