cmake_minimum_required(VERSION 2.8.4)
project(knapsack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++14")

add_executable(knapsack_dp dynamic_programming.cpp)
add_executable(knapsack_bt backtracking.cpp)