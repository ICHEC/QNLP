################################################################################
cmake_minimum_required (VERSION 3.10)
set(QNLP_LOGO "

              ▄▄▄▄    █   ▄▄   ▄ ▄      ▄▄▄▄▄   █
             ▄▀  ▀▄   █   █▀▄  █ █      █   ▀█   █
             █    █   █   █ █▄ █ █      █▄▄▄█▀    █
             █    █   █   █  █ █ █      █         █
              █▄▄█▀   █   █   ██ █▄▄▄▄▄ █        █
                 █    █                         █

")
message(${QNLP_LOGO})
################################################################################
# Out-of-source builds only
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

################################################################################
project (QNLP)

################################################################################
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Include cmake directory for use of custom Find<packagename> commands
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

################################################################################

set(QHIPSTER $ENV{QHIPSTER_DIR} CACHE INTERNAL "" FORCE) 
set(QHIPSTER_LIB $ENV{QHIPSTER_DIR_LIB}/qHiPSTER.a CACHE INTERNAL "" FORCE) 
set(QHIPSTER_INC $ENV{QHIPSTER_DIR_INC} CACHE INTERNAL "" FORCE) 


################################################################################
set(CMAKE_CXX_FLAGS 
     "-lmkl_rt \
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
     -O3        \
     -lmkl_blacs_intelmpi_ilp64"
)

################################################################################
add_definitions(
    "-DUSE_MM_MALLOC \
     -DMKL_ILP64 \
     -DCOLLECT_TIME \
     -DSTANDALONE \
     -DOPENQU_HAVE_MPI \
     -DINTELQS_HAS_MPI" 
)


################################################################################
# Include the directories with sources
add_subdirectory(third_party)
message("BASE: ${CMAKE_MODULE_PATH}")
add_subdirectory(modules)

################################################################################
# Include third_party depedencies
add_library(${PROJECT_NAME} INTERFACE)


################################################################################
# Select options taken from https://cliutils.gitlab.io/modern-cmake
################################################################################
#find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )

if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

################################################################################
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

################################################################################
find_package(MPI REQUIRED)

################################################################################
#add_executable(cswap_test ./tests/test_control_swap.cpp)

include_directories("${QHIPSTER_INC}")

################################################################################
ADD_LIBRARY(qhipster STATIC IMPORTED)
SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")


add_subdirectory(tests)

################################################################################
#target_link_libraries(cswap_test qhipster)
#target_link_libraries(cswap_test qhipster Catch2::Catch2 CLI11::CLI11)

