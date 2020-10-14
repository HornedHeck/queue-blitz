#ifndef ACS_2_CONDITIONALQUEUE_H
#define ACS_2_CONDITIONALQUEUE_H

#include "Queue.h"
#include <condition_variable>
#include <queue>
#include <thread>

using std::unique_lock;
using std::condition_variable;
using std::mutex;

class ConditionalQueue : public Queue {

    std::queue<uint8_t> items;
    mutex items_w_m;
    condition_variable items_check_w_cv;
    mutex items_r_m;
    condition_variable items_check_r_cv;
    size_t max_size;

    void pop_internal(uint8_t &val);


public:
    explicit ConditionalQueue(size_t maxSize);

    void push(uint8_t val) override;

    bool pop(uint8_t &val) override;

};

#endif //ACS_2_CONDITIONALQUEUE_H
