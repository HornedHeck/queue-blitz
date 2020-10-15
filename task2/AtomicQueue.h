#ifndef ACS_2_ATOMICQUEUE_H
#define ACS_2_ATOMICQUEUE_H

#include "Queue.h"
#include <atomic>
#include <condition_variable>
#include <queue>
#include <array>
#include <thread>

using std::atomic;
using std::unique_lock;
using std::condition_variable;
using std::mutex;
using std::array;

enum State {
    FREE, STORING, LOADING
};

template<size_t max_size>
class AtomicQueue : public Queue {

private:
    atomic<State> w_state;
    atomic<State> r_state;
    std::queue<uint8_t> items;
    mutex r_signal;
    mutex w_signal;
    condition_variable r_cv;
    condition_variable w_cv;

public:

    AtomicQueue() {
        w_state.store(FREE);
        r_state.store(FREE);
    }

    void push(uint8_t val) override {
        while (true) {
            State exp_state = FREE;
            if (w_state.compare_exchange_strong(exp_state, STORING)) {
                if (items.size() < max_size) {
                    items.push(val);
                    r_cv.notify_one();
                    w_state.store(FREE);
                    if (items.size() < max_size) w_cv.notify_one();
                    r_cv.notify_one();
                    return;
                }
                w_state.store(FREE);
            }
            {
                unique_lock<mutex> lock(w_signal);
                w_cv.wait_for(lock, microseconds(10));
            }
        }
    }

    bool pop(uint8_t &val) override {

        while (true) {
            State exp_state = FREE;
            if (r_state.compare_exchange_strong(exp_state, LOADING)) {
                if (!items.empty()) {
                    val = items.front();
                    items.pop();
                    r_state.store(FREE);
                    if (!items.empty()) r_cv.notify_one();
                    w_cv.notify_one();
                    return true;
                }
                r_state.store(FREE);
            }
            std::cv_status status;
            {
                unique_lock<mutex> lock(r_signal);
                status = r_cv.wait_for(lock, wait_duration);
            }
            if (status == std::cv_status::timeout) break;
        }

        return false;
    }

};


#endif //ACS_2_ATOMICQUEUE_H
