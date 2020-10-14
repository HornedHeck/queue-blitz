#ifndef ACS_2_MUTEXQUEUE_H
#define ACS_2_MUTEXQUEUE_H

#include "Queue.h"
#include <queue>
#include <mutex>
#include <thread>

using std::queue;
using std::mutex;
using std::timed_mutex;
using std::thread;

class MutexQueue : public Queue {
private:
    queue<uint8_t> items;
    mutex write_m;
    timed_mutex read_m;

public:
    void push(uint8_t val) override;

    bool pop(uint8_t &val) override;

};


#endif //ACS_2_MUTEXQUEUE_H
