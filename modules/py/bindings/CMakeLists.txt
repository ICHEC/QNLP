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


# Install the bound module into the QNLP Python environment
configure_file(setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py )

add_custom_target(PipInstaller ALL DEPENDS _PyQNLPSimulator)
add_custom_command(TARGET PipInstaller
    # Run after all other rules within the target have been executed
    POST_BUILD
    COMMAND pip install .
    COMMENT "Installing Python bindings into local installation"
    VERBATIM
)
