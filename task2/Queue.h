#ifndef ACS_2_QUEUE_H
#define ACS_2_QUEUE_H

#include <cstdint>
#include <chrono>

using namespace std::chrono;

class Queue {

protected:

protected:
    milliseconds wait_duration = milliseconds(1);

public:

    virtual void push(uint8_t val) = 0;

    virtual bool pop(uint8_t &val) = 0;

};

#endif //ACS_2_QUEUE_H
