cmake_minimum_required(VERSION 3.12)

project(qnlp_demo_ncu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exe_demo_nCU demo_nqubit_ControlledUnitary.cpp)
target_link_libraries(exe_demo_nCU qnlp_simulator qhipster)
