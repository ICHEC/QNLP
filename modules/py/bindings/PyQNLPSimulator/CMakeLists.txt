cmake_minimum_required(VERSION 3.12)
project(qnlp_python_module)

if(${ENABLE_PYTHON})
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_CXX_STANDARD 14)
        set(PYBIND11_CPP_STANDARD -std=c++14)
    else()
        set(CMAKE_CXX_STANDARD 17)
        set(PYBIND11_CPP_STANDARD -std=c++17)
    endif()
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -faligned-new")

    find_package(Python COMPONENTS Interpreter Development)

    add_library(_PyQNLPSimulator SHARED Simulator_pybind11.cpp)
    target_link_libraries(_PyQNLPSimulator PRIVATE pybind11::module qnlp_simulator)
    set_target_properties(_PyQNLPSimulator PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}" SUFFIX "${PYTHON_MODULE_EXTENSION}")

    #Ensure the Python module init and install files are copied to build directory
    configure_file(__init__.py ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)
endif()
