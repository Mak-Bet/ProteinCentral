cmake_minimum_required(VERSION 3.18)
project(igraph_part)
find_package(igraph REQUIRED)
add_executable(igraph_part igraph_part.cpp)
target_link_libraries(igraph_part PUBLIC igraph::igraph)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)