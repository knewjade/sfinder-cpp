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
            return !notWorking();
        }

        [[nodiscard]] bool notWorking() const {
            return isAborted_ || isTerminated_;
        }

    private:
        std::atomic<bool> isTerminated_ = false;
        std::atomic<bool> isAborted_ = false;
    };

    using Runnable = std::function<void(const TaskStatus &)>;

    template<typename T>
    using Callable = std::function<T(const TaskStatus &)>;

    class Tasks {
    public:
        void push(const Runnable &runnable) {
            {
                std::lock(mutexForQueue_, mutexForAbort_);
                std::lock_guard<std::mutex> lk1(mutexForQueue_, std::adopt_lock);
                std::lock_guard<std::mutex> lk2(mutexForAbort_, std::adopt_lock);

                if (status_.notWorking()) {
                    throw std::runtime_error("Not working");
                }

                queue_.push(runnable);
                counter += 1;
            }

            conditionForQueue_.notify_one();
            conditionForSleep_.notify_one();
        }

        void execute() {
            while (true) {
                Runnable runnable;

                {
                    std::unique_lock<std::mutex> guard(mutexForQueue_);
                    conditionForQueue_.wait(guard, [this] { return status_.notWorking() || !queue_.empty(); });

                    if (queue_.empty()) {
                        if (status_.notWorking()) {
                            if (status_.terminated()) {
                                // All tasks completed, so finish pool
                                return;
                            }

                            // All tasks completed, so go to sleep
                            conditionForAbort_.notify_all();
                            conditionForSleep_.wait(guard, [this] {
                                return status_.working() || status_.terminated();
                            });
                        }

                        // Working but not found task or after sleeping
                        continue;
                    }

                    // Execute task

                    runnable = queue_.front();
                    queue_.pop();
                }

                runnable(status_);

                {
                    std::lock_guard<std::mutex> guard(mutexForAbort_);
                    counter -= 1;
                }
            }
        }

        void abort() {
            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                status_.abort();
            }

            conditionForQueue_.notify_all();
            conditionForSleep_.notify_all();

            // sleep until completed all tasks
            {
                std::unique_lock<std::mutex> guard(mutexForAbort_);
                conditionForAbort_.wait(guard, [this] {
                    return counter == 0;
                });
            }

            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                status_.resume();
            }

            conditionForQueue_.notify_all();
            conditionForSleep_.notify_all();
        }

        void shutdown() {
            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                status_.abort();
            }

            conditionForQueue_.notify_all();
            conditionForSleep_.notify_all();

            // sleep until completed all tasks
            {
                std::unique_lock<std::mutex> guard(mutexForAbort_);
                conditionForAbort_.wait(guard, [this] {
                    return counter == 0;
                });
            }

            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                status_.terminate();
            }

            conditionForQueue_.notify_all();
            conditionForSleep_.notify_all();
            conditionForAbort_.notify_all();
        }

        void shutdownNow() {
            {
                std::lock_guard<std::mutex> guard(mutexForQueue_);
                status_.terminate();

                counter -= queue_.size();

                std::queue<Runnable> empty{};
                std::swap(queue_, empty);
            }

            conditionForQueue_.notify_all();
            conditionForSleep_.notify_all();
            conditionForAbort_.notify_all();
        }

        bool terminated() const {
            return status_.terminated();
        }

    private:
        std::mutex mutexForQueue_;
        std::mutex mutexForAbort_;

        TaskStatus status_{};

        int counter = 0;
        std::queue<Runnable> queue_{};

        std::condition_variable conditionForQueue_{};
        std::condition_variable conditionForSleep_{};
        std::condition_variable conditionForAbort_{};
    };

    /**
     * This class is NOT thread-safe.
     */
    class ThreadPool {
    public:
        explicit ThreadPool(int n) : tasks_(std::make_unique<Tasks>()) {
            start(n);
        }

        ~ThreadPool() {
            if (tasks_->terminated()) {
                return;
            }

            tasks_->shutdownNow();
            for (auto &thread : threads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads_.clear();
        }

        // Execute the task
        void execute(const Runnable &runnable) {
            if (tasks_->terminated()) {
                throw std::runtime_error("Thread pool is terminated");
            }

            tasks_->push(runnable);
        }

        // Execute the task returning result.
        template<typename R>
        std::future<R> execute(const Callable<R> &callable) {
            if (tasks_->terminated()) {
                throw std::runtime_error("Thread pool is terminated");
            }

            auto task = std::make_shared<std::packaged_task<R(const TaskStatus &)>>(callable);
            tasks_->push([task](const TaskStatus &status) {
                (*task)(status);
            });
            return std::move(task->get_future());
        }

        // Change current status to "aborted" and wait all tasks is completed.
        // The task is notified of the "aborted" status via TaskStatus, but attempts to complete all processing.
        // How long the task ends depends on the task implementation.
        void abort() {
            if (tasks_->terminated()) {
                return;
            }

            tasks_->abort();
        }

        // After abort tasks and wait they are completed, change current status to "Terminated".
        // Thread pool cannot be operated after shutdown.
        void shutdown() {
            if (tasks_->terminated()) {
                return;
            }

            tasks_->shutdown();
            for (auto &thread : threads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads_.clear();
        }

        // Change the number of threads.
        // Running tasks are aborted and wait for changes to complete.
        void changeThreadCount(int n) {
            shutdown();
            tasks_ = std::make_unique<Tasks>();
            start(n);
        }

    private:
        void start(int n) {
            assert(0 < n);
            threads_.clear();
            for (int count = 0; count < n; ++count) {
                threads_.emplace_back(std::thread([this]() {
                    tasks_->execute();
                }));
            }
        }

        std::vector<std::thread> threads_{};
        std::unique_ptr<Tasks> tasks_;
    };
}

#endif //FINDER_THREAD_POOLS_HPP