cmake_minimum_required(VERSION 3.12)
project(qnlp_modules LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS    db;
                            gate_ops;
                            encoding;
                            py;
                            simulator;
                            #qubit_extended;
                            #util;
                            test
                            io;
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

###############################################################################

#file(GLOB_RECURSE mod_src_list "*.cpp" )
#file(GLOB_RECURSE mod_inc_list "*.hpp" )

#add_library(qnlp_modules STATIC ${mod_src_list} ${mod_inc_list})
#target_include_directories(qnlp_modules PUBLIC ${mod_inc_list})

#find_package (SQLite3 REQUIRED)
#if (SQLITE3_FOUND)
#    target_include_directories(qnlp_modules PUBLIC ${SQLITE3_INCLUDE_DIRS})
#    target_link_libraries (qnlp_modules ${SQLITE3_LIBRARIES})
#endif (SQLITE3_FOUND)

