cmake_minimum_required(VERSION 3.12)

project(qnlp_demo_ncu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exe_nCU nqubit_ControlledUnitary.cpp)
target_link_libraries(exe_nCU qnlp_simulator qhipster)
#target_include_directories(exe_nCU PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
