cmake_minimum_required (VERSION 3.0)

add_executable(qnlp_qft_test test_qft_qnlp.cpp)
target_link_libraries(qnlp_qft_test qnlp_qubit_extended qnlp_util qnlp_ncu qhipster)

