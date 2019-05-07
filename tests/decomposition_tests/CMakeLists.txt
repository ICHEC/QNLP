cmake_minimum_required (VERSION 3.0)

add_executable(nqc_test test_nqubit_decompose.cpp)
target_link_libraries(nqc_test qnlp_qubit_extended qnlp_ncu qhipster)
