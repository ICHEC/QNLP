cmake_minimum_required (VERSION 3.0)

add_executable(exe_yrot yRotations.cpp)
target_link_libraries(exe_yrot qnlp_qubit_extended qnlp_ncu qhipster)



