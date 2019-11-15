cmake_minimum_required(VERSION 3.12)

project(qnlp_diffusion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_diffusion INTERFACE)
target_include_directories(qnlp_diffusion INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_diffusion OBJECT test_diffusion.cpp)
    target_link_libraries(test_diffusion Catch2::Catch2 qnlp_simulator qhipster qnlp_diffusion)
    target_include_directories(test_diffusion PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
endif()
