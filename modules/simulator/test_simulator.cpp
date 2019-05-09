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
TEST_CASE("Intel-QS simulator creation","[simulator]"){
    for(std::size_t num_qubits = 1; num_qubits<16; num_qubits++){
        DYNAMIC_SECTION("Register creation with " << std::to_string(num_qubits) << " qubits"){
            IntelSimulator sim_test(num_qubits);
            REQUIRE(sim_test.getNumQubits() == num_qubits);
            REQUIRE(sim_test.getQubitRegister().NumQubits() == num_qubits);
        }
    }
}

TEST_CASE("Pauli operators"){
    IntelSimulator sim(1);
    auto& reg = sim.getQubitRegister();

    SECTION("Initial state as |0>"){
        REQUIRE(reg[0].real() == 1.);
        REQUIRE(reg[0].imag() == 0.);
        REQUIRE(reg[1].real() == 0.);
        REQUIRE(reg[1].imag() == 0.);
    }
    SECTION("Pauli X |0>"){
        sim.applyGateX(0);
        REQUIRE(reg[0].real() == 0.);
        REQUIRE(reg[0].imag() == 0.);
        REQUIRE(reg[1].real() == 1.);
        REQUIRE(reg[1].imag() == 0.);
    }
    SECTION("Pauli Y |0>"){
        sim.applyGateY(0);
        REQUIRE(reg[0].real() == 0.);
        REQUIRE(reg[0].imag() == 0.);
        REQUIRE(reg[1].real() == 0.);
        REQUIRE(reg[1].imag() == 1.);
    }
    SECTION("Pauli Z |0>"){
        sim.applyGateZ(0);
        REQUIRE(reg[0].real() == 1.);
        REQUIRE(reg[0].imag() == 0.);
        REQUIRE(reg[1].real() == 0.);
        REQUIRE(reg[1].imag() == 0.);
    }
}

/**
 * @brief User defined main required for this instance, as openqu::mpi::Environment destructor calls MPI_Finalize.
 * As we require MPI until the end of the test session, it must be created before the tests, and destroyed after
 * all have finished.
 */
int main( int argc, char* argv[] ) {
  openqu::mpi::Environment env(argc, argv);

  int result = Catch::Session().run( argc, argv );

  return result;
}
