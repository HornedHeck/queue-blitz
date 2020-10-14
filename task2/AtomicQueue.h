#ifndef ACS_2_ATOMICQUEUE_H
#define ACS_2_ATOMICQUEUE_H

#include "Queue.h"
#include <atomic>
#include <condition_variable>
#include <queue>

using std::atomic;
using std::unique_lock;
using std::condition_variable;
using std::mutex;

class AtomicQueue : public Queue {

private:

public:
    explicit AtomicQueue(size_t maxSize);

    void push(uint8_t val) override;

    bool pop(uint8_t &val) override;

};


#endif //ACS_2_ATOMICQUEUE_H
