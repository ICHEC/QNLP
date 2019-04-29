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
#target_link_libraries(tp_test.cpp Catch2::Catch2 CLI11::CLI11)