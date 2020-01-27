cmake_minimum_required(VERSION 3.12)
project(qnlp_modules LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS
#                            corpus_encoding;
#                            decomposition_tests;
#                            entanglement_test;
#                            installation_tests;
#                            nqubit_ControlledU;
#                            test_arithmetic;
#                            test_qft;
#                            yRot
)

foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

###############################################################################

#file(GLOB_RECURSE mod_src_list "*.cpp" )
#file(GLOB_RECURSE mod_inc_list "*.hpp" )

#add_library(qnlp_modules STATIC ${mod_src_list} ${mod_inc_list})
#target_include_directories(qnlp_modules PUBLIC ${mod_inc_list})
