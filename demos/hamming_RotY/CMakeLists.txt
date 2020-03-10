cmake_minimum_required(VERSION 3.12)

project(qnlp_demo_hamming_RotY)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exe_demo_hamming_RotY demo_hamming_RotY.cpp)
target_link_libraries(exe_demo_hamming_RotY qnlp_simulator intel_qs)
