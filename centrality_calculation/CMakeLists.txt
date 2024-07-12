cmake_minimum_required(VERSION 3.18)
project(centrality_calculation)
find_package(igraph REQUIRED)
add_executable(centrality_calculation centrality_calculation.cpp)
target_link_libraries(centrality_calculation PUBLIC igraph::igraph)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)