#ifndef FINDER_THREAD_POOLS_HPP
#define FINDER_THREAD_POOLS_HPP

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <future>
#include <atomic>

namespace finder {
    class TaskStatus {
    public:
        void resume() {
            isAborted_ = false;
        }

        void abort() {
            isAborted_ = true;
        }

        void terminate() {
            isTerminated_ = true;
        }

        [[nodiscard]] bool terminated() const {
            return isTerminated_;
        }

        [[nodiscard]] bool aborted() const {
            return isAborted_;
        }

        [[nodiscard]] bool working() const {
            return !(isAborted_ || isTerminated_);
        }

    private:
        bool isTerminated_ = false;
        bool isAborted_ = false;
    };

    using Runnable = std::function<void(const TaskStatus &)>;

    template<typename T>
    using Callable = std::function<T(const TaskStatus &)>;

    class Tasks {
    public:
        void push(const Runnable &runnable) {
            if (!status_.working()) {
                throw std::runtime_error("Not working");
            }

            counter += 1;

            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                queue_.push(runnable);
            }

            conditionForQueue_.notify_one();
        }

        void execute() {
            while (true) {
                Runnable runnable;

                {
                    std::unique_lock<std::mutex> guard(mutexForQueue_);
                    conditionForQueue_.wait(guard, [this] { return !status_.working() || !queue_.empty(); });

                    if (!status_.working() && queue_.empty()) {
                        if (status_.terminated()) {
                            return;
                        }

                        conditionForAbort_.notify_all();
                        conditionForQueue_.wait(guard, [this] { return status_.working() || status_.terminated(); });
                        continue;
                    }

                    runnable = queue_.front();
                    queue_.pop();
                }

                runnable(status_);

                counter -= 1;
            }
        }

        void abort() {
            status_.abort();

            // sleep until completed all tasks
            {
                std::unique_lock<std::mutex> guard(mutexForAbort_);
                conditionForAbort_.wait(guard, [this] { return counter == 0; });
            }

            status_.resume();
            conditionForQueue_.notify_all();
        }

        void shutdown() {
            status_.terminate();
            conditionForQueue_.notify_all();
        }

        void shutdownNow() {
            status_.abort();
            status_.terminate();
            conditionForQueue_.notify_all();
        }

    private:
        TaskStatus status_{};
        std::atomic<int> counter = std::atomic<int>(0);
        std::queue<Runnable> queue_{};

        std::mutex mutexForQueue_;
        std::condition_variable conditionForQueue_{};

        std::mutex mutexForAbort_;
        std::condition_variable conditionForAbort_{};
    };

    class ThreadPool {
    public:
        explicit ThreadPool(int n) {
            for (int count = 0; count < n; ++count) {
                threads_.emplace_back(std::thread([this]() {
                    tasks_.execute();
                }));
            }
        }

        void execute(const Runnable &runnable) {
            tasks_.push(runnable);
        }

        template<typename R>
        std::future<R> execute(const Callable<R> &callable) {
            auto task = std::make_shared<std::packaged_task<R(const TaskStatus &)>>(callable);
            tasks_.push([task](const TaskStatus &status) {
                (*task)(status);
            });
            return std::move(task->get_future());
        }

        void abort() {
            tasks_.abort();
        }

        void shutdown() {
            tasks_.shutdown();
            for (auto &thread : threads_) {
                thread.join();
            }
        }

        void shutdownNow() {
            tasks_.shutdownNow();
            for (auto &thread : threads_) {
                thread.join();
            }
        }

    private:
        std::vector<std::thread> threads_{};
        Tasks tasks_{};
    };
}

#endif //FINDER_THREAD_POOLS_HPP

