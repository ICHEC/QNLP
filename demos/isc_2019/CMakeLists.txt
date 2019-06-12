cmake_minimum_required(VERSION 3.12)

project(qnlp_isc_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(encode_corpus EncodeCorpus.cpp)
target_link_libraries(encode_corpus qnlp_db qhipster qnlp_simulator CLI11::CLI11)
