cmake_minimum_required (VERSION 3.0)

add_executable(exe_qft_test qft_test.cpp)
target_link_libraries(exe_qft_test qhipster)
target_include_directories(exe_qft_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${QHIPSTER_INC})

