cmake_minimum_required (VERSION 3.0)

add_executable(exe_measurement measurement_test.cpp)
target_link_libraries(exe_measurement qnlp_qubit_extended qhipster)
