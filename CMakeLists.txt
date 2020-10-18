cmake_minimum_required(VERSION 3.17)
project(ACS_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(ACS_2
        main.cpp
        task1/atomic_executor.h
        task1/blocking_executor.h
        task1/mutex_executor.h
        task2/Queue.h
        task2/MutexQueue.cpp
        task2/MutexQueue.h
        task2/ConditionalQueue.cpp
        task2/ConditionalQueue.h
        task2/AtomicQueue.cpp
        task2/AtomicQueue.h)

set(THREADS_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
target_link_libraries(ACS_2 Threads::Threads)