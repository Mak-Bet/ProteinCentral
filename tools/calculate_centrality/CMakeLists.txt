cmake_minimum_required(VERSION 3.18)
project(calculate_centrality)
find_package(igraph REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/../common)

add_executable(calculate_centrality calculate_centrality.cpp)
target_link_libraries(calculate_centrality PUBLIC igraph::igraph)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)