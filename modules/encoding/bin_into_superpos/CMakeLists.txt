cmake_minimum_required(VERSION 3.12)

project(qnlp_binary_encoding)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_BIN_ENCODE_INC ${PROJECT_SOURCE_DIR} PARENT_SCOPE)

#find_package(IntelQS REQUIRED)
add_library(qnlp_binencode INTERFACE)
target_include_directories(qnlp_binencode INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    #add_library(test_binencode OBJECT test_encode_binary.cpp)
    #target_link_libraries(test_binencode Catch2::Catch2 qnlp_binencode qnlp_simulator)
endif()
