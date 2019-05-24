cmake_minimum_required(VERSION 3.12)

project(qnlp_arithmetic)
#message("ARITHMETIC: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(IntelQS REQUIRED)
#find_package(QNLP_QFT)

set(QNLP_ARITHMETIC_PROJECT_SOURCES sum_sub.cpp sum_sub.hpp)

add_library(qnlp_arithmetic STATIC ${QNLP_ARITHMETIC_PROJECT_SOURCES})
target_link_libraries(qnlp_arithmetic qnlp_qft ${QHIPSTER_LIB})
#target_include_directories(qnlp_arithmetic PUBLIC ${QHIPSTER_INC} ../qft)

target_include_directories(qnlp_arithmetic PUBLIC ./)

