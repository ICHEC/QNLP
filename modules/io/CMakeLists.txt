cmake_minimum_required(VERSION 3.12)
project(qnlp_io LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS
                            db;
                            GateWriter
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################

