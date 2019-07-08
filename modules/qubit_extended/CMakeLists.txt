cmake_minimum_required(VERSION 3.12)
project(qnlp_qubit_extended)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qnlp_qubit_extended STATIC QubitCircuit.hpp QubitCircuit.cpp ${QHIPSTER_INC})
target_include_directories(qnlp_qubit_extended PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC} qnlp_simulator)
target_link_libraries(qnlp_qubit_extended qnlp_ncu qhipster qnlp_simulator)
