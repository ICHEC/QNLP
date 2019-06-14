//#define CATCH_CONFIG_RUNNER

#include "hamming.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;
using namespace Catch::Matchers;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class HammingDistance<IntelSimulator>;


TEST_CASE("Test Hamming distance routine","[Hamming]"){
    SECTION("Testing distance calc"){

    }
}

/*
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
*/
