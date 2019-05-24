cmake_minimum_required(VERSION 3.12)
project(qnlp_modules LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS
                            superposition_encoding;
#                            hamming_distance;
#                            corpus_encoding;
#                            superposition_encoding_optimal_num_qubits;
#                            nn_hamming_distance;
#                            superposition_encoding;
#                            nn_superposition_encoding;
#                            db_tests;
#                            installation_tests;
#                            nn_hamming_distance_Schuld;
#                            test_arithmetic;
#                            decomposition_tests;
#                            measurement;
#                            nqubit_ControlledU;
#                            test_qft;
#                            entanglement_test;
#                            measure_probs;
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
