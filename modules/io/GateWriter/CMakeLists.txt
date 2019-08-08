cmake_minimum_required(VERSION 3.12)

project(qnlp_gatewriter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_gatewriter INTERFACE)
target_include_directories(qnlp_gatewriter INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)