cmake_minimum_required (VERSION 3.0)

add_library(QNLP_gate_ops_ncu STATIC ncu.cpp)

target_include_directories(QNLP_gate_ops_ncu PRIVATE ${CMAKE_CURRENT_LIST_DIR})
