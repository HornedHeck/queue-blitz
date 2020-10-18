#include <mutex>
#include "blocking_executor.h"

using std::mutex;

class MutexTasksExecutor : public BlockingExecutor {

private:
    size_t index;
    mutex m_index;

    size_t get_and_increment() {
        size_t index;
        m_index.lock();
        index = this->index;
        this->index++;
        m_index.unlock();
        return index;
    }

protected:
    void reset() override {
        index = 0;
    }

private:

    void thread_action(bool is_delayed) override {
        size_t index;
        while ((index = get_and_increment()) < tasks_size) {
            tasks[index] += 1;
            if (is_delayed) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            }
        }
    }

};
