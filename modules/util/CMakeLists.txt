cmake_minimum_required(VERSION 3.12)

project(qnlp_util)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(IntelQS REQUIRED)
add_library(qnlp_util STATIC util.hpp util.cpp ${QHIPSTER_INC})
target_include_directories(qnlp_util PUBLIC ${QHIPSTER_INC})
