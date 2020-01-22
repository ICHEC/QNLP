cmake_minimum_required(VERSION 3.12)
project(qnlp_demos LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS
                            isc_2019;
                            nqubit_ControlledU;
                            encoding;
                            hamming_RotY;
                            hamming_similarity_example;
)

foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

###############################################################################