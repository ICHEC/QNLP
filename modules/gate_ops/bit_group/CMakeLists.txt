cmake_minimum_required(VERSION 3.12)

project(qnlp_bitgroup)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_bitgroup INTERFACE)
target_include_directories(qnlp_bitgroup INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_bitgroup OBJECT test_bit_group.cpp)
    target_link_libraries(test_bitgroup qnlp_bitgroup Catch2::Catch2 qnlp_simulator)
    target_include_directories(test_bitgroup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
