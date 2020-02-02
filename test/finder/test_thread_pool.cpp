#include "gtest/gtest.h"

#include "finder/thread_pool.hpp"

#include <chrono>
#include <future>
#include <vector>

namespace finder {
    using namespace std::literals::string_literals;

    class ThreadPoolTest : public ::testing::Test {
    };

    void sleep200ms(const TaskStatus &status) {
        if (status.aborted()) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    auto sleep200msCallable(int i) {
        return [i](const TaskStatus &status) {
            if (status.aborted()) {
                return -i;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            return i;
        };
    }

    TEST_F(ThreadPoolTest, shutdown) {
        auto pool = ThreadPool(3);

        auto start = std::chrono::system_clock::now();

        for (int i = 0; i < 7; ++i) {
            pool.execute(sleep200ms);
        }

        pool.shutdown();

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        ASSERT_GE(time, 600);
        ASSERT_LT(time, 615);
    }

    TEST_F(ThreadPoolTest, shutdownNow) {
        auto pool = ThreadPool(3);

        auto start = std::chrono::system_clock::now();

        for (int i = 0; i < 7; ++i) {
            pool.execute(sleep200ms);
        }

        pool.shutdownNow();

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        ASSERT_GE(time, 200);
        ASSERT_LT(time, 215);
    }

    TEST_F(ThreadPoolTest, reusability) {
        auto pool = ThreadPool(3);

        for (int i = 0; i < 100; ++i) {
            auto futures = std::vector<std::future<int>>(7);
            for (int j = 0; j < futures.size(); ++j) {
                Callable<int> callable = sleep200msCallable(j);
                futures[j] = pool.execute(callable);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            pool.abort();

            int k = 0;
            for (; k < 3; ++k) {
                EXPECT_EQ(futures[k].get(), k);
            }
            for (; k < futures.size(); ++k) {
                EXPECT_EQ(futures[k].get(), -k);
            }
        }

        pool.shutdown();

        EXPECT_EQ(1, 1);
    }

    TEST_F(ThreadPoolTest, executeAfterShutdown) {
        auto pool = ThreadPool(3);
        pool.shutdown();

        ASSERT_THROW(pool.execute(sleep200ms), std::runtime_error);
    }
}
