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
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

#Selectively build the test object and binary
option(ENABLE_TESTS "Enable QNLP tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()

################################################################################
# Include cmake directory for use of custom Find<packagename> commands
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

################################################################################

set(QHIPSTER $ENV{QHIPSTER_DIR} CACHE INTERNAL "" FORCE)
set(QHIPSTER_LIB $ENV{QHIPSTER_DIR_LIB}/qHiPSTER.a CACHE INTERNAL "" FORCE)
set(QHIPSTER_INC $ENV{QHIPSTER_DIR_INC} CACHE INTERNAL "" FORCE)

################################################################################


################################################################################
find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

################################################################################

find_package(MPI REQUIRED)
#include_directories(SYSTEM ${MPI_INCLUDE_PATH})
#target_link_libraries(my_mpi_target ${MPI_C_LIBRARIES})
################################################################################

# Set CMAKE_CXX_FLAGS specific for OS
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS
        "-L/${CMAKE_CURRENT_SOURCE_DIR}/third_party/install/intel-qnlp_conda/envs/intel-qnlp/lib \
        -fopenmp                \
        -I/${CMAKE_CURRENT_SOURCE_DIR}/third_party/install/intel-qnlp_conda/envs/intel-qnlp/include                 \
        -lmpi                   \
        "
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS
        "-qopenmp                       \
        -lmkl_blacs_intelmpi_ilp64      \
        "
    )
endif ()

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}     \
     -lmkl_rt               \
      -lpthread             \
     -ldl                   \
     -liomp5                \
     -Wall                  \
     -lmkl_cdft_core        \
     -lmkl_intel_ilp64      \
     -lmkl_core             \
     -lmkl_intel_thread     \
     -g \
     -O3 \
     "
)

option(ENABLE_AVX512 "Enable Skylake AVX512 build tuning" OFF)
if(${ENABLE_AVX512})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -march=skylake-avx512 \
        "
    )
endif()

option(ENABLE_AVX512_ZMM "Enable Skylake AVX512 ZMM high usage" OFF)
if(${ENABLE_AVX512_ZMM})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -qopt-zmm-usage=high \
        "
    )
endif()

option(ENABLE_IPO "Enable inter-procedural optimisations" OFF)
if(${ENABLE_IPO})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -ipo \
        "
    )
endif()

option(ENABLE_IPO "Enable fast math" OFF)
if(${ENABLE_FAST_MATH})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} \
        -fp-model fast=${ENABLE_FAST_MATH} \
        "
    )
endif()




################################################################################

# Add OS specific definitions
#if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    add_definitions(
        "-DINTELQS_HAS_MPI"
    )
endif ()


add_definitions(
    "-DUSE_MM_MALLOC \
     -DMKL_ILP64 \
     -DCOLLECT_TIME \
     -DSTANDALONE \
     -DOPENQU_HAVE_MPI \ 
     -DVIRTUAL_INTERFACE \
     -DLATEX_OUTPUT"
)

################################################################################
# Add Intel-QS library as target
ADD_LIBRARY(qhipster STATIC IMPORTED)
SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")
target_include_directories(qhipster INTERFACE ${QHIPSTER_INC})

################################################################################
# Include the directories with sources
add_subdirectory(third_party)
#message("BASE: ${CMAKE_MODULE_PATH}")
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
find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

################################################################################
find_package(MPI)

################################################################################
#add_executable(cswap_test ./tests/test_control_swap.cpp)

#include_directories("${QHIPSTER_INC}")

################################################################################
add_subdirectory(tests)
add_subdirectory(demos)

################################################################################
#target_link_libraries(cswap_test qhipster)
#target_link_libraries(cswap_test qhipster Catch2::Catch2 CLI11::CLI11)

