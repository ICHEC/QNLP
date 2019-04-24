cmake_minimum_required (VERSION 3.0)
project (QNLP)

#include(intel)

#list(APPEND CMAKE_MODULE_PATH "/ichec/work/ichec001/modules")

#Require C++11 for Intel-QS_tmp
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QHIPSTER $ENV{QHIPSTER_DIR}) 
set(QHIPSTER_LIB $ENV{QHIPSTER_DIR_LIB}/qHiPSTER.a) 
set(QHIPSTER_INC $ENV{QHIPSTER_DIR_INC}) 

set(QUBIT_EXTENDED_INC "${CMAKE_CURRENT_SOURCE_DIR}/../../Qubit_Extended/include")
set(QUBIT_EXTENDED_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../Qubit_Extended/lib/libQubit_Extended.a")

#set(QUBIT_EXTENDED_INC "/ichec/home/staff/mdoyle/intel-qnlp/Qubit_Extended/include")
#set(QUBIT_EXTENDED_LIB "/ichec/home/staff/mdoyle/intel-qnlp/Qubit_Extended/lib/libQubit_Extended.a")

set(CMAKE_CXX_FLAGS 
    "-DUSE_MM_MALLOC \
     -lmkl_rt \
     -lpthread \
     -ldl \
     $ENV{MKLROOT}/lib/intel64/libmkl_scalapack_ilp64.a \
     -Wl,--start-group \
     $ENV{MKLROOT}/lib/intel64/libmkl_cdft_core.a \
     $ENV{MKLROOT}/lib/intel64/libmkl_intel_ilp64.a \
     $ENV{MKLROOT}/lib/intel64/libmkl_intel_thread.a \
     $ENV{MKLROOT}/lib/intel64/libmkl_core.a \
     $ENV{MKLROOT}/lib/intel64/libmkl_blacs_intelmpi_ilp64.a \
     -Wl,--end-group \
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

#set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(MPI REQUIRED)


include_directories("${QUBIT_EXTENDED_INC}")
include_directories("${QHIPSTER_INC}")

#ADD_LIBRARY(qubitExtended STATIC IMPORTED)
#SET_TARGET_PROPERTIES(qubitExtended PROPERTIES IMPORTED_LOCATION "${QUBIT_EXTENDED_LIB}")

# Adds n qubit controlled U operator
include_directories("../../include")
add_executable(exe_hamming_distance hamming_distance.cpp ../../src/nqubit_decompose.cpp ../../include/nqubit_decompose.hpp)

ADD_LIBRARY(qhipster STATIC IMPORTED)
SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")

#target_link_libraries(exe_hamming_distance qubitExtended qhipster)
target_link_libraries(exe_hamming_distance qhipster)



