cmake_minimum_required(VERSION 3.12)
project(qnlp_encoding LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_MODULES_SUBDIRS
                            bin_into_superpos;
)
foreach(MOD ${QNLP_MODULES_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################

