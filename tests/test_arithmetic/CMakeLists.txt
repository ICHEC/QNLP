cmake_minimum_required (VERSION 3.0)

add_executable(test_arithmetic test_arithmetic.cpp)
target_link_libraries(test_arithmetic qnlp_qubit_extended qnlp_util qnlp_ncu qhipster)

