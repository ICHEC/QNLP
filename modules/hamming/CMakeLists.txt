cmake_minimum_required(VERSION 3.12)

project(qnlp_hamming)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_HAMMING_INC ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

#find_package(IntelQS REQUIRED)
add_library(qnlp_hamming INTERFACE)
target_include_directories(qnlp_hamming INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_hamming OBJECT test_hamming.cpp)
    target_link_libraries(test_hamming Catch2::Catch2 qnlp_hamming qnlp_simulator)
endif()
