cmake_minimum_required(VERSION 3.12)
project(qnlp_third_party LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
option(IqsFPIC "" ON)
set(QNLP_THIRD_PARTY_SUBDIRS    intel-qs;
                                CLI11;
                                Catch2;
)
set(CLI11_SINGLE_FILE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Catch2/contrib)

if(${ENABLE_PYTHON})
    # mpi4py build and install
    set(QNLP_THIRD_PARTY_SUBDIRS ${QNLP_THIRD_PARTY_SUBDIRS} pybind11)
    find_package(MPI REQUIRED)
    add_custom_target(mpi4pyInstaller ALL DEPENDS _PyQNLPSimulator)
    add_custom_command(TARGET mpi4pyInstaller
        # Run after all other rules within the target have been executed
        POST_BUILD
        COMMAND CC=${MPI_CXX_COMPILER} CXX=${MPI_CXX_COMPILER} python setup.py build --mpicc=${MPI_CXX_COMPILER}
        COMMAND python setup.py install
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/mpi4py
        COMMENT "Building mpi4py using the available MPI environment"
        VERBATIM
    )
endif()

###############################################################################

foreach(MOD ${QNLP_THIRD_PARTY_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

###############################################################################
