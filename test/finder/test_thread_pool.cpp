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

    void sleep200msWithoutAborting(const TaskStatus &status) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::function<int(const TaskStatus &)> sleep200msCallable(int i) {
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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        pool.shutdown();

        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        ASSERT_GE(time, 200);
        ASSERT_LT(time, 220);
    }

    TEST_F(ThreadPoolTest, reusability) {
        auto pool = ThreadPool(3);

        for (int i = 0; i < 1000; ++i) {
            std::cout << "#" << i << std::endl;
            auto futures = std::vector<std::future<int>>(7);
            for (int j = 0; j < futures.size(); ++j) {
                futures[j] = pool.execute(sleep200msCallable(j));
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
    }

    TEST_F(ThreadPoolTest, executeAfterShutdown) {
        auto pool = ThreadPool(3);
        pool.shutdown();

        ASSERT_THROW(pool.execute(sleep200ms), std::runtime_error);
    }

    TEST_F(ThreadPoolTest, changeThreadCount) {
        int threadCount = 1;
        auto pool = ThreadPool(threadCount);

        for (int i = 0; i < 1000; ++i) {
            std::cout << "#" << i << std::endl;
            auto futures = std::vector<std::future<int>>(7);
            for (int j = 0; j < futures.size(); ++j) {
                futures[j] = pool.execute(sleep200msCallable(j));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            pool.abort();

            for (auto &future : futures) {
                int n = future.get();  // Check finished
                EXPECT_EQ(n - n, 0);
            }

            if (i % 10 == 0) {
                std::cout << "> change" << std::endl;
                pool.changeThreadCount(threadCount);
                threadCount = ((threadCount + 1) % 5) + 1;
            }
        }

        pool.shutdown();
    }
}
