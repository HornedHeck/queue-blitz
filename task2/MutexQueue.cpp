#include "MutexQueue.h"
#include <future>

void MutexQueue::push(uint8_t val) {
    write_m.lock();
    items.push(val);
    write_m.unlock();
}

bool MutexQueue::pop(uint8_t &val) {
    read_m.lock();
    bool res = false;
    if (items.empty()) {
        bool *is_canceled = new bool(false);
        auto read_f = std::async(&MutexQueue::is_read_available, this, is_canceled);
        auto status = read_f.wait_for(wait_duration);
        *is_canceled = true;
        if (status == std::future_status::ready) {
            res = true;
        }
    } else {
        res = true;
    }
    if (res) {
        write_m.lock();
        val = items.front();
        items.pop();
        write_m.unlock();
    }
    read_m.unlock();
    return res;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "LoopDoesntUseConditionVariableInspection"

bool MutexQueue::is_read_available(const bool *canceled) {
    while (!*canceled) {
        if (!items.empty()) {
            return true;
        }
        std::this_thread::sleep_for(nanoseconds(5));
    }
    delete canceled;
    return false;
}

#pragma clang diagnostic pop
