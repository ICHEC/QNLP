cmake_minimum_required(VERSION 3.12)

project(qnlp_oracle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PYBIND11_CPP_STANDARD -std=c++1z)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -faligned-new")

find_package (Python COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

add_library(PythonSimulator SHARED Simulator_pybind11.cpp)
#target_include_directories(qnlp_python INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(PythonSimulator PRIVATE pybind11::module qnlp_simulator)
set_target_properties(PythonSimulator PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
