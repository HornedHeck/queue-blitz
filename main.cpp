#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include "task2/MutexQueue.h"
#include "task2/ConditionalQueue.h"
#include "task2/AtomicQueue.h"
#include "task1/blocking_executor.h"
#include "task1/mutex_executor.h"
#include "task1/atomic_executor.h"

using namespace std::chrono;
using std::cout;
using std::endl;
using std::vector;

void run_task1_test(
        BlockingExecutor *executor,
        int thread_count,
        bool is_delayed,
        bool with_check = false
) {
    size_t size = 1024L * 1024L;
    auto src = new int8_t[size]{0};
    auto arg = new int8_t[size]{0};

    auto started = std::chrono::high_resolution_clock::now();
    executor->start_blocking(arg, size, is_delayed, thread_count);
    auto done = std::chrono::high_resolution_clock::now();

    std::cout << "Thread count: "
              << thread_count
              << std::endl
              << "Execution time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count()
              << "ms"
              << std::endl
              << std::endl;

    if (with_check) {
        bool isSuccess = true;
        size_t i;
        for (i = 0; i < size; ++i) {

            if (arg[i] != src[i] + 1) {
                isSuccess = false;
                src[i] = arg[i];
                break;
            }
            src[i] = arg[i];
        }
        if (isSuccess) {
            std::cout << thread_count << " - Successful\n\n";
        } else {
            std::cout << thread_count << " - Fail at " << i << std::endl;
        }
    }
}

void task1(const vector<int> &thread_counts, bool is_delayed) {

    auto mutex_executor = new MutexTasksExecutor();

    cout << "Thread counts: ";
    for (int c : thread_counts) {
        cout << c << " ";
    }
    cout << endl << "Is delayed: " << is_delayed << endl;

    cout << "Mutex executor:" << endl;
    for (auto count : thread_counts) {
        run_task1_test(mutex_executor, count, is_delayed);
    }

    auto atomic_executor = new AtomicTaskExecutor();
    cout << "Atomic executor:" << endl;
    for (auto count : thread_counts) {
        run_task1_test(atomic_executor, count, is_delayed);
    }
}

mutex out;

void producer(size_t size, Queue *queue, int id) {
    auto started = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size; ++i) {
        queue->push(1);
    }
    auto done = std::chrono::high_resolution_clock::now();
    out.lock();
    cout
            << "Producer " << id << " - "
            << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count()
            << "ms" << endl;
    out.unlock();
}


size_t consumer(Queue *queue, bool *is_produced, int id) {
    uint8_t res = 0;
    size_t consumed = 0;
    auto started = std::chrono::high_resolution_clock::now();
    while (true) {
        auto read = queue->pop(res);
        if (read) {
            ++consumed;
        }
        if (*is_produced && !read) break;
    }
    auto done = std::chrono::high_resolution_clock::now();
    out.lock();
    cout
            << "Consumer " << id << " - "
            << consumed << " - "
            << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count()
            << "ms" << endl;
    out.unlock();
    return consumed;
}

void task2(Queue *queue, size_t producers_count, size_t consumers_count, size_t size = 4 * 1024 * 1024) {

    vector<thread> producers;
    producers.reserve(producers_count);
    for (int i = 0; i < producers_count; ++i) {
        producers.emplace_back(producer, size, queue, i);
    }

    bool *is_produced = new bool(false);

    vector<std::future<size_t>> consumers;
    consumers.reserve(producers_count);
    for (int i = 0; i < consumers_count; ++i) {
        consumers.push_back(std::async(consumer, queue, is_produced, i));
    }

    for (auto &producer_t : producers) {
        producer_t.join();
    }
    *is_produced = true;

    size_t consumed = 0;
    for (auto &consumer_t : consumers) {
        consumed += consumer_t.get();
    }

    cout
            << "Produced: " << size * producers_count << endl
            << "Consumed: " << consumed << endl
            << "Result: " << (size * producers_count == consumed ? "OK" : "Fail") << endl;
}

void run_task2_tests(Queue *queue, vector<int> &producers, vector<int> &consumers) {
    for (int p : producers) {
        for (int c : consumers) {
            cout << "P " << p << " C " << c << endl;
            task2(queue, p, c);
        }
    }
}

typedef std::map<std::string, std::string> args_map;

#define TASK "--task"
#define SUBTASK "--subtask"
#define THREADS "--threads"
#define PRODUCERS "--producers"
#define CONSUMERS "--consumers"
#define SIZE "--size"

int get_number(args_map &args, const std::string &name, int def = -1) {
    if (args.contains(name)) {
        return std::stoi(args[name]);
    }
    return def;
}

vector<int> get_numbers(args_map &args, const std::string &name, const vector<int> &def) {
    if (args.contains(name)) {
        vector<int> res{std::stoi(args[name])};
        return res;
    }
    return def;
}

void parse_task_1(args_map &args) {
    int subtask = get_number(args, SUBTASK);
    vector<int> threads = get_numbers(args, THREADS, vector{1, 2, 4, 16});
    switch (subtask) {
        case 2:
            task1(threads, false);
            break;
        case 3:
            task1(threads, true);
            break;
        default:
            cout << "Wrong subtask" << endl;
    }
}

void print_p_c(vector<int> &producers, vector<int> &consumers) {
    cout << "Producers: ";
    for (int p : producers) cout << " " << p;
    cout << endl << "Consumers: ";
    for (int c : consumers) cout << " " << c;
    cout << endl;
}

void parse_task_2(args_map &args) {
    int subtask = get_number(args, SUBTASK);
    vector<int> producers = get_numbers(args, PRODUCERS, vector{1, 2, 4});
    vector<int> consumers = get_numbers(args, CONSUMERS, vector{1, 2, 4});
    vector<int> sizes = get_numbers(args, SIZE, vector{1, 4, 16});
    Queue *queue;
    switch (subtask) {
        case 1:
            print_p_c(producers, consumers);
            queue = new MutexQueue();
            run_task2_tests(queue, producers, consumers);
            break;
        case 2:
            print_p_c(producers, consumers);
            for (int size : sizes) {
                queue = new ConditionalQueue(size);
                run_task2_tests(queue, producers, consumers);
            }
            break;
        case 3:
            print_p_c(producers, consumers);
            for (int size : sizes) {
                queue = new AtomicQueue(size);
                run_task2_tests(queue, producers, consumers);
            }
            break;
        default:
            cout << "Wrong subtask" << endl;
    }
}

// Format
// --task
// --subtask
// --size -- query size
// --threads -- thread count
// --producers -- producers count
// --consumers -- consumers count
int main(int argc, char *argv[]) {

    args_map args;
    for (int i = 1; i < argc; i += 2) {
        args.insert(std::pair(std::string(argv[i]), std::string(argv[i + 1])));
    }

    int task = get_number(args, TASK);
    switch (task) {
        case 1:
            parse_task_1(args);
            break;
        case 2:
            parse_task_2(args);
            break;
        default:
            cout << "Wrong task number";
    }
    return 0;
}
