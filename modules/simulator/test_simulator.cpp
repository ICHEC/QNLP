/**
 * @file test_simulator.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Tests for the simulator interface. 
 * @version 0.1
 * @date 2019-05-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define CATCH_CONFIG_RUNNER

#include "catch2/catch.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"

using namespace QNLP;

/**
 * @brief Tests creating a simulator (in this case, the Intel-QS), for a variety of different qubit counts, up to a max limit.
 * 
 */
TEST_CASE("Testing Intel-QS simulator creation","[simulator]"){
    std::string label = "";
    for(std::size_t num_qubits = 1; num_qubits<16; num_qubits++){
        label = "Testing " + std::to_string(num_qubits) + " qubits";
        SECTION(label){
            IntelSimulator sim_test(num_qubits);
        }
    }
}

/**
 * @brief User defined main required for this instance, as openqu::mpi::Environment destructor calls MPI_Finalize.
 * As we require MPI until the end of the test session, it must be created before the tests, and destroyed after
 * all have finished only.
 */
int main( int argc, char* argv[] ) {
  openqu::mpi::Environment env(argc, argv);

  int result = Catch::Session().run( argc, argv );

  return result;
}
