cmake_minimum_required (VERSION 3.0)
project (QNLP)

#Require C++11 for Intel-QS
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
     -DSTANDALONE \
     -DOPENQU_HAVE_MPI \
     -DMKL_ILP64 \
     -lm \
     -Wall"
)
set(QHIPSTER $ENV{PWD}/../../../Intel-QS/build) 

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(MPI REQUIRED)

add_executable(cswap test_control_swap.cpp)

include_directories("${QHIPSTER}/include")
# link_directories("${QHIPSTER}/lib/intel64")

#find_library(QHIPSTER_LIBRARY
#    NAMES qHiPSTER
#    HINTS "${CMAKE_PREFIX_PATH}/curl/lib"
#)

ADD_LIBRARY(qhipster STATIC IMPORTED)
SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER}/lib/intel64/qHiPSTER.a")
target_link_libraries(cswap qhipster)
#target_link_libraries(cswap MPI_C)
