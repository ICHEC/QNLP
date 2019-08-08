cmake_minimum_required(VERSION 3.12)

project(qnlp_db)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QNLP_DB_FILES db_helper.cpp db_helper.hpp corpus_utils.cpp corpus_utils.hpp CACHE INTERNAL "" FORCE)

find_package (SQLite3 REQUIRED)
if (SQLITE3_FOUND)
    add_library(qnlp_db STATIC ${QNLP_DB_FILES})
    target_link_libraries (qnlp_db ${SQLITE3_LIBRARIES})
    target_include_directories(qnlp_db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SQLITE3_INCLUDE_DIRS})

else()
    message( SEND_ERROR "SQLite3 required for database access. libqnlp_db will not be built" )
endif ()

#enable_testing()
if(${CMAKE_TESTING_ENABLED})
    add_library(test_db OBJECT test_db.cpp)
    target_link_libraries(test_db Catch2::Catch2 qnlp_db)
endif()
