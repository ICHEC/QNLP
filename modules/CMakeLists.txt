cmake_minimum_required(VERSION 3.12)
project(qnlp_modules LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS    gate_ops;
                            encoding;
                            hamming;
                            py;
                            simulator;
                            utils;
                            test
                            io;
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()

###############################################################################

