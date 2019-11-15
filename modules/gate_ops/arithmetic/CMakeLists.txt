cmake_minimum_required(VERSION 3.12)

project(qnlp_arithmetic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_arithmetic INTERFACE)
target_include_directories(qnlp_arithmetic INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_arithmetic OBJECT test_arithmetic.cpp)
    target_link_libraries(test_arithmetic qnlp_arithmetic Catch2::Catch2 qnlp_simulator)
    target_include_directories(test_arithmetic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
