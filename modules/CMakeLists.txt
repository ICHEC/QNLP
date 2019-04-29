cmake_minimum_required(VERSION 3.12)
project(qnlp_modules LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS    db;
                            gate_ops;
                            py;
                            qubit_extended;
                            simulator;
                            util
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################