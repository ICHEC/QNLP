cmake_minimum_required(VERSION 3.12)

project(qnlp_qft)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_QFT_INC ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

#find_package(IntelQS REQUIRED)
add_library(qnlp_qft INTERFACE)
target_include_directories(qnlp_qft INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_qft OBJECT test_qft.cpp)
    target_link_libraries(test_qft Catch2::Catch2 qnlp_qft qnlp_simulator)
endif()
