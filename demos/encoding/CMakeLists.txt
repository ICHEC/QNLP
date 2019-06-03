cmake_minimum_required(VERSION 3.12)

project(qnlp_demo_encoding)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exe_demo_encoding demo_encoding.cpp)
target_link_libraries(exe_demo_encoding qnlp_simulator qhipster)
