cmake_minimum_required(VERSION 3.0.0...3.13)
project(generate_list_graph VERSION 0.1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/../common)

add_executable(generate_list_graph generate_list_graph.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
