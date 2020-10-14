#include "MutexQueue.h"

void MutexQueue::push(uint8_t val) {
    write_m.lock();
    items.push(val);
    write_m.unlock();
}

bool MutexQueue::pop(uint8_t &val) {
    bool is_locked = read_m.try_lock_for(wait_duration);
    if (is_locked) {
        write_m.lock();
        bool is_empty = items.empty();
        if (!is_empty) {
            val = items.front();
            items.pop();
        }
        write_m.unlock();
        read_m.unlock();
        return !is_empty;
    }
    return false;
}
