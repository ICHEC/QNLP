cmake_minimum_required(VERSION 3.12)
project(qnlp_gateOps LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS    #arithmetic;
                            diffusion;
                            ncu;
                            qft;
                            oracle;
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################
#add_dependencies(qnlp_arithmetic qnlp_qft)

