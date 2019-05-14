
#define CATCH_CONFIG_RUNNER

#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}

//#define CATCH_CONFIG_MAIN
//#include "catch2/catch.hpp"
