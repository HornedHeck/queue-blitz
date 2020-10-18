#ifndef ACS_2_ATOMIC_EXECUTOR_H
#define ACS_2_ATOMIC_EXECUTOR_H

#include "blocking_executor.h"
#include <atomic>

using std::atomic;

class AtomicTaskExecutor : public BlockingExecutor {

private:

    atomic<size_t> index{0};

    void thread_action(bool is_delayed) override {
        size_t index;
        while ((index = this->index.fetch_add(1)) < tasks_size) {
            tasks[index] += 1;
            if (is_delayed) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            }
        }
    }

    void reset() override {
        index.store(0);
    }
};


#endif //ACS_2_ATOMIC_EXECUTOR_H
