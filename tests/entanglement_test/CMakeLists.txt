cmake_minimum_required (VERSION 3.0)

add_executable(test_bell_vs_super test_bell_vs_super.cpp)
target_link_libraries(test_bell_vs_super qnlp_qubit_extended qnlp_util qnlp_ncu qhipster)

