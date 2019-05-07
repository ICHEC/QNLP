cmake_minimum_required (VERSION 3.0)

add_executable(exe_nCU nqubit_ControlledUnitary.cpp)
target_link_libraries(exe_nCU qnlp_qubit_extended qnlp_ncu qhipster)
