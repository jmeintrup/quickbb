cmake_minimum_required(VERSION 3.16)
project(quickBB)

set(CMAKE_CXX_STANDARD 20)

add_executable(quickBB main.cpp graph.hpp quickbb.hpp _types.hpp graph_io.hpp tree.hpp)