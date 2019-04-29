cmake_minimum_required (VERSION 3.0)
project (QNLP)

#include(intel)

#list(APPEND CMAKE_MODULE_PATH "/ichec/work/ichec001/modules")
#include(bigmpi)

#Require C++11 for Intel-QS_tmp
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CURRENT PATH  ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS 
    "-DUSE_MM_MALLOC \
     -lmkl_rt \
     -lpthread \
     -ldl \
     -liomp5 \
     -lm \
     -qopenmp \
     -Wall \
     -lmkl_cdft_core \
     -lmkl_intel_ilp64 \
     -lmkl_core \
     -lmkl_intel_thread \
     -lmkl_blacs_intelmpi_ilp64 \
     -march=skylake-avx512      \
     -DMKL_ILP64 \
     -DCOLLECT_TIME \
     -DSTANDALONE \
     -DOPENQU_HAVE_MPI \
     -DINTELQS_HAS_MPI \
     -DNOREPA_HAS_MPI"   # Not required for anything outside of Intel-QS/tests
)


set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=skylake-avx512")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(MPI REQUIRED)

include_directories("${QHIPSTER_INC}")
#include_directories("/ichec/home/staff/mdoyle/intel-qnlp/Qubit_Extended/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
ADD_LIBRARY(Qubit_Extended STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/QubitCircuit.cpp")
