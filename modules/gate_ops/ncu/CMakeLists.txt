cmake_minimum_required(VERSION 3.12)

project(qnlp_ncu)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(IntelQS REQUIRED)
add_library(qnlp_ncu STATIC ncu.hpp ncu.cpp ${QHIPSTER_INC})
target_include_directories(qnlp_ncu PUBLIC ${QHIPSTER_INC} ${CMAKE_CURRENT_SOURCE_DIR})
