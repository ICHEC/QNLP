//#define CATCH_CONFIG_NOSTDOUT
#define CATCH_CONFIG_RUNNER

#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"
#include <fstream> 
#include <iostream> 

/**
 * @brief Unit testing main() function. Runs all unit testing using Cath22
 * 
 * @param argc number of command line arguments 
 * @param argv command line arguments separated by spaces
 * @return int Returns 0 upon successful execution
 */
int main( int argc, char* argv[] ) {
    qhipster::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}

//#define CATCH_CONFIG_MAIN
//#include "catch2/catch.hpp"