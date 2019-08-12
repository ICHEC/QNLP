cmake_minimum_required(VERSION 3.12)
project(qnlp_python LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_PYTHON_SUBDIRS    PyQNLPSimulator;
)
foreach(MOD ${QNLP_PYTHON_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

configure_file(setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py )
