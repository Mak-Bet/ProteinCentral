cmake_minimum_required(VERSION 3.0.0...3.13)
project(my_graph_creation VERSION 0.1.0 LANGUAGES CXX)

add_executable(my_graph_creation my_graph_creation.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
