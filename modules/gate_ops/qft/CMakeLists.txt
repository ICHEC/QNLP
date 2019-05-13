cmake_minimum_required(VERSION 3.12)

project(qnlp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_QFT_INC ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

#find_package(IntelQS REQUIRED)
add_library(qnlp_qft STATIC qft.cpp qft.hpp)
target_include_directories(qnlp_qft PUBLIC ${PROJECT_SOURCE_DIR} ${QHIPSTER_INC})
target_link_libraries(qnlp_qft ${QHIPSTER_LIB} qnlp_simulator)
