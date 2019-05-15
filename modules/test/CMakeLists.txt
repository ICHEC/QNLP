cmake_minimum_required(VERSION 3.12)

project(qnlp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_TESTING_ENABLED})
    add_executable(tests test_main.cpp)
    
    add_library(qhipster STATIC IMPORTED)
    set_target_properties(qhipster PROPERTIES IMPORTED_LOCATION "${QHIPSTER_LIB}")
    
    target_link_libraries(tests Catch2::Catch2 test_db test_simulator test_ncu test_qft qhipster)

    include(CTest)
    include(${CMAKE_SOURCE_DIR}/third_party/Catch2/contrib/Catch.cmake)
    
    catch_discover_tests(tests)
endif()
