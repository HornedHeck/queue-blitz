#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include "task2/MutexQueue.h"
#include "task2/ConditionalQueue.h"
#include "task2/AtomicQueue.h"

using namespace std::chrono;
using std::cout;
using std::endl;
using std::vector;

void producer(size_t size, Queue *queue) {
    for (size_t i = 0; i < size; ++i) {
        queue->push(1);
    }
}

size_t consumer(Queue *queue, bool *is_produced) {
    uint8_t res = 0;
    size_t consumed = 0;
//    for (; !*is_produced || queue->pop(res); ++consumed);
    while (true) {
        auto read = queue->pop(res);
        if (read) {
            ++consumed;
        }
        if (*is_produced && !read) break;
    }
    return consumed;
}

void task2(Queue *queue, size_t producers_count, size_t consumers_count, size_t size = 4 * 1024 * 1024) {

    auto start = system_clock::now();

    vector<thread> producers;
    producers.reserve(producers_count);
    for (int i = 0; i < producers_count; ++i) {
        producers.emplace_back(producer, size, queue);
    }

    bool *is_produced = new bool(false);

    vector<std::future<size_t>> consumers;
    consumers.reserve(producers_count);
    for (int i = 0; i < consumers_count; ++i) {
        consumers.push_back(std::async(consumer, queue, is_produced));
    }

    for (auto &producer_t : producers) {
        producer_t.join();
    }
    *is_produced = true;

    cout << "Produced\n";

    size_t consumed = 0;
    for (auto &consumer_t : consumers) {
        consumed += consumer_t.get();
    }

    auto end = system_clock::now();

    cout
            << "Produced: " << size * producers_count << endl
            << "Consumed: " << consumed << endl
            << "Elapsed: " << duration_cast<milliseconds>(end - start).count() << endl;

}

int main() {

//    Queue *queue = new MutexQueue();
//    Queue *queue = new ConditionalQueue(1);
    Queue *queue = new AtomicQueue<16>();
    task2(queue, 1, 4, 1024 * 1024 * 4);
    return 0;
}
