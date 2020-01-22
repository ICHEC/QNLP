cmake_minimum_required(VERSION 3.12)

project(qnlp_simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(QNLP_SIMULATOR_FILES IntelSimulator.cpp sim_factory.cpp Simulator.hpp CACHE INTERNAL "" FORCE)
set(QNLP_SIMULATOR_FILES IntelSimulator.cpp Simulator.hpp CACHE INTERNAL "" FORCE)

add_library(qnlp_simulator STATIC ${QNLP_SIMULATOR_FILES})

target_include_directories(qnlp_simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})
target_link_libraries(qnlp_simulator intel_qs qnlp_ncu qnlp_oracle qnlp_diffusion qnlp_qft qnlp_arithmetic qnlp_gatewriter qnlp_qft qnlp_binencode qnlp_hamming_RotY_amplification qnlp_utils)

if(${CMAKE_TESTING_ENABLED})
    add_library(test_simulator OBJECT test_simulator.cpp ${QNLP_SIMULATOR_FILES})
    target_link_libraries(test_simulator Catch2::Catch2 qnlp_simulator intel_qs qnlp_ncu qnlp_diffusion qnlp_oracle qnlp_qft qnlp_binencode qnlp_hamming_RotY_amplification qnlp_utils)
endif()
