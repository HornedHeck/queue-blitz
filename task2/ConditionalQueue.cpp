#include <future>
#include "ConditionalQueue.h"

void ConditionalQueue::push(uint8_t val) {
    {
        unique_lock<mutex> lock(items_w_m);
        if (items.size() < max_size) {
            items.push(val);
        } else {
            items_check_w_cv.wait(lock);
            items.push(val);
        }
    }
    items_check_r_cv.notify_one();
}

bool ConditionalQueue::pop(uint8_t &val) {
    unique_lock<mutex> lock(items_r_m);
    if (items.empty()) {
        {
            auto status = items_check_r_cv.wait_for(lock, wait_duration);
            if (status == std::cv_status::no_timeout && !items.empty()) {
                pop_internal(val);
                return true;
            } else {
                return false;
            }
        }
    } else {
        pop_internal(val);
        return true;
    }
}

void ConditionalQueue::pop_internal(uint8_t &val) {
    items_w_m.lock();
    val = items.front();
    items.pop();
    items_w_m.unlock();
    items_check_w_cv.notify_one();
}

ConditionalQueue::ConditionalQueue(size_t maxSize) : max_size(maxSize) {}
