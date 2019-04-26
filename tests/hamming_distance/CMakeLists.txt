cmake_minimum_required (VERSION 3.0)

# Adds n qubit controlled U operator
include_directories("../../include")
add_executable(exe_hamming_distance hamming_distance.cpp ../../src/nqubit_decompose.cpp ../../include/nqubit_decompose.hpp)

ADD_LIBRARY(qhipster STATIC IMPORTED)
SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")

#target_link_libraries(exe_hamming_distance qubitExtended qhipster)
target_link_libraries(exe_hamming_distance qhipster)



