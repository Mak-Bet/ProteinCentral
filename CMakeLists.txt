cmake_minimum_required(VERSION 3.0.0...3.13)
project(residue_links_creation VERSION 0.1.0 LANGUAGES C CXX)

add_executable(residue_links_creation main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
