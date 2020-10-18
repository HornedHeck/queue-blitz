#ifndef ACS_2_BLOCKING_EXECUTOR_H
#define ACS_2_BLOCKING_EXECUTOR_H

#include <thread>
#include <vector>
#include <cstdint>
#include <cstdlib>

using std::thread;
using std::vector;

class BlockingExecutor {
protected:
    vector<thread> threads;

    int8_t *tasks;
    size_t tasks_size;

    virtual void thread_action(bool is_delayed) = 0;

    virtual void reset() = 0;

public:
    virtual void start_blocking(int8_t *tasks, size_t tasks_size, bool is_delayed, int thread_count) {
        this->tasks = tasks;
        this->tasks_size = tasks_size;
        threads.clear();
        reset();
        for (int i = 0; i < thread_count; ++i) {
            thread thr(&BlockingExecutor::thread_action, this, is_delayed);
            threads.push_back(std::move(thr));
        }
        for (int i = 0; i < thread_count; ++i) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
    }

};

#endif //ACS_2_BLOCKING_EXECUTOR_H
