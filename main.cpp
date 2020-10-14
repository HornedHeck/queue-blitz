#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include "task2/MutexQueue.h"

using namespace std::chrono;
using std::cout;
using std::endl;
using std::vector;

void producer(size_t size, Queue *queue) {
    for (size_t i = 0; i < size; ++i) {
        queue->push(1);
    }
}

size_t consumer(Queue *queue) {
    uint8_t res = 0;
    size_t consumed = 0;
    for (; queue->pop(res); ++consumed);
    return consumed;
}

int main() {

    Queue *queue = new MutexQueue();

    size_t size = 1024 * 1024;
    size_t producer_count = 2;
    size_t consumers_count = 4;

    auto start = system_clock::now();

    vector<thread> producers;
    producers.reserve(producer_count);
    for (int i = 0; i < producer_count; ++i) {
        producers.emplace_back(producer, size, queue);
    }

    vector<std::future<size_t>> consumers;
    consumers.reserve(producer_count);
    for (int i = 0; i < consumers_count; ++i) {
        consumers.push_back(std::async(consumer, queue));
    }

    for (auto &producer_t : producers) {
        producer_t.join();
    }

    size_t consumed = 0;
    for (auto &consumer_t : consumers) {
        consumed += consumer_t.get();
    }

    auto end = system_clock::now();

    cout
            << "Produced: " << size * producer_count << endl
            << "Consumed: " << consumed << endl
            << "Elapsed: " << duration_cast<milliseconds>(end - start).count() << endl;

    return 0;
}
