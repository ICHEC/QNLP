cmake_minimum_required (VERSION 3.0)

add_executable(exe_hamming_distance hamming_distance.cpp)
target_link_libraries(exe_hamming_distance qnlp_qubit_extended qnlp_ncu qhipster)
target_include_directories(exe_hamming_distance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
