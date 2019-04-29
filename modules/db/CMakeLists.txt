cmake_minimum_required(VERSION 3.12)

project(qnlp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlpDB STATIC db_helper.cpp db_helper.hpp corpus_utils.cpp corpus_utils.hpp)