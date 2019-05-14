cmake_minimum_required(VERSION 3.12)

project(qnlp_ncu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(IntelQS REQUIRED)
#add_library(qnlp_ncu STATIC ncu.hpp ncu.cpp ${QHIPSTER_INC})
#target_include_directories(qnlp_ncu PUBLIC ${QHIPSTER_INC} ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(qnlp_ncu qnlp_simulator qhipster)


add_executable(test_ncu test_ncu.cpp)

add_library(qhipster STATIC IMPORTED)
set_target_properties(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")
target_link_libraries(test_ncu Catch2::Catch2 qnlp_simulator qhipster)# qnlp_ncu)
