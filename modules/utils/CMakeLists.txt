cmake_minimum_required(VERSION 3.12)

project(qnlp_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_utils INTERFACE)
target_include_directories(qnlp_utils INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)