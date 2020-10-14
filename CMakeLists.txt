cmake_minimum_required(VERSION 3.17)
project(ACS_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(ACS_2 main.cpp task2/Queue.h task2/MutexQueue.cpp task2/MutexQueue.h task2/ConditionalQueue.cpp task2/ConditionalQueue.h)

set(THREADS_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
target_link_libraries(ACS_2 Threads::Threads)