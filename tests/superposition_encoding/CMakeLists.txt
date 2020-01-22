cmake_minimum_required (VERSION 3.10)
#set (CMAKE_CXX_STANDARD 17)

add_executable(exe_superposition_encoding superposition_encoding.cpp)
target_link_libraries(exe_superposition_encoding qnlp_ncu qnlp_qubit_extended qhipster )
