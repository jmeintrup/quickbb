cmake_minimum_required(VERSION 3.16)
project(quickBB)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
add_executable(quickBB main.cpp graph.hpp quickbb.hpp _types.hpp graph_io.hpp tree.hpp)
set_target_properties(quickBB PROPERTIES COMPILE_FLAGS ${CMAKE_CXX_FLAGS})