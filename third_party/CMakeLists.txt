cmake_minimum_required(VERSION 3.12)
project(qnlp_third_party LANGUAGES CXX)

###############################################################################
# Include all nested sources directories
###############################################################################
set(QNLP_THIRD_PARTY_SUBDIRS    CLI11;
                                Catch2;
)
foreach(MOD ${QNLP_THIRD_PARTY_SUBDIRS})
    add_subdirectory(${MOD})
endforeach()
###############################################################################
set(CLI11_SINGLE_FILE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Catch2/contrib)
