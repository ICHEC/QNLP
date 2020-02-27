cmake_minimum_required(VERSION 3.12)
project(qnlp_gateOps LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS    arithmetic;
                            diffusion;
                            ncu;
                            qft;
                            oracle;
                            bit_group;
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################
