cmake_minimum_required(VERSION 3.12)

project(qnlp_hamming_RotY_amplification)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_HAMMING_INC ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

add_library(qnlp_hamming_RotY_amplification INTERFACE)
target_include_directories(qnlp_hamming_RotY_amplification INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_hamming_RotY_amplification OBJECT test_hamming_RotY_amplification.cpp)
    target_link_libraries(test_hamming_RotY_amplification Catch2::Catch2 qnlp_hamming_RotY_amplification qnlp_simulator)
endif()
