cmake_minimum_required(VERSION 3.12)

project(qnlp_ncu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_ncu INTERFACE)
target_include_directories(qnlp_ncu INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(qnlp_ncu INTERFACE qnlp_utils)


if(${CMAKE_TESTING_ENABLED})
    add_library(test_ncu OBJECT test_ncu.cpp)
    target_link_libraries(test_ncu Catch2::Catch2 qnlp_simulator intel_qs)
    target_include_directories(test_ncu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
endif()
