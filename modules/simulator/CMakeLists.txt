cmake_minimum_required(VERSION 3.12)

project(qnlp_simulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

#set(QNLP_SIMULATOR_FILES IntelSimulator.cpp sim_factory.cpp Simulator.hpp CACHE INTERNAL "" FORCE)
set(QNLP_SIMULATOR_FILES IntelSimulator.cpp Simulator.hpp CACHE INTERNAL "" FORCE)

add_library(qnlp_simulator STATIC ${QNLP_SIMULATOR_FILES})
message("SIM: ${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(qnlp_simulator PUBLIC ${QHIPSTER_INC} ${CMAKE_CURRENT_SOURCE_DIR})



#ADD_LIBRARY(qhipster STATIC IMPORTED)
#SET_TARGET_PROPERTIES(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")

#target_link_libraries (qnlp_simulator qhipster)

#find_package (SQLite3 REQUIRED)
#if (SQLITE3_FOUND)
#    add_library(qnlp_db STATIC ${QNLP_DB_FILES})
#    include_directories(${SQLITE3_INCLUDE_DIRS})
#    target_link_libraries (qnlp_db ${SQLITE3_LIBRARIES})
#else()
#    message( SEND_ERROR "SQLite3 required for database access. libqnlp_db will not be built" )
#endif ()

#enable_testing()
#find_package(Catch2 REQUIRED)

add_executable(test_simulator test_simulator.cpp ${QNLP_SIMULATOR_FILES})

add_library(qhipster STATIC IMPORTED)
set_target_properties(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")
target_link_libraries(test_simulator Catch2::Catch2 qnlp_simulator qhipster)
