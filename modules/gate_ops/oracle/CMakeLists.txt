cmake_minimum_required(VERSION 3.12)

project(qnlp_oracle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_oracle INTERFACE)
target_include_directories(qnlp_oracle INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_oracle OBJECT test_oracle.cpp)
    target_link_libraries(test_oracle qnlp_oracle Catch2::Catch2 qnlp_simulator iqs )
    target_include_directories(test_oracle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
endif()
