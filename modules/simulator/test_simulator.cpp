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
#include <memory>

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

    SECTION("State |0>"){
        SECTION("Initial state as |0>"){
            REQUIRE(reg[0] == std::complex<double>(1.,0.) );
            REQUIRE(reg[1] == std::complex<double>(0.,0.) );
        }
        SECTION("Pauli X |0>"){
            sim.applyGateX(0);
            REQUIRE(reg[0].real() == std::complex<double>(0.,0.));
            REQUIRE(reg[1].real() == std::complex<double>(1.,0.));
        }
        SECTION("Pauli Y |0>"){
            sim.applyGateY(0);
            REQUIRE(reg[0] == std::complex<double>(0.,0.));
            REQUIRE(reg[1] == std::complex<double>(0.,1.));
        }
        SECTION("Pauli Z |0>"){
            sim.applyGateZ(0);
            REQUIRE(reg[0] == std::complex<double>(1.,0.) );
            REQUIRE(reg[1] == std::complex<double>(0.,0.) );
        }
    }
    SECTION("State |1>"){
        sim.applyGateX(0);
        SECTION("Pauli X |1>"){
            sim.applyGateX(0);
            REQUIRE(reg[0] == std::complex<double>(1.,0.));
            REQUIRE(reg[1] == std::complex<double>(0.,0.));
        }
        SECTION("Pauli Y |1>"){
            sim.applyGateY(0);
            REQUIRE(reg[0] == std::complex<double>(0.,-1.));
            REQUIRE(reg[1] == std::complex<double>(0.,0.));
        }
        SECTION("Pauli Z |1>"){
            sim.applyGateZ(0);
            REQUIRE(reg[0] == std::complex<double>(0.,0.));
            REQUIRE(reg[1] == std::complex<double>(-1.,0.));
        }
    }
}

/**
 *  Consider using virtual for non-standard functions, and CRTP for everything else.
 *  Default functions should be as fast as possible, with the others being only
 *  rarely needed, thus performance isn't as critical.
 */

TEST_CASE("Simulator interface"){
    SECTION("ISimulator virtual base pointer"){
        ISimulator *s1 = new IntelSimulator(8) ;//createSimulator(8);
        REQUIRE(s1->getNumQubits() == 8);
        delete s1;
    }
    SECTION("SimulatorGeneral<IntelSimulator> pointer"){
        SimulatorGeneral<IntelSimulator> *s2 = new IntelSimulator(8) ;//createSimulator(8);
        REQUIRE(s2->getNumQubits() == 8);
        delete s2; 
    }
    SECTION("unique_ptr<ISimulator> to IntelSimulator object"){
        std::unique_ptr<ISimulator> s3(new IntelSimulator(8));
        REQUIRE(s3->getNumQubits() == 8);
    }
    SECTION("unique_ptr<ISimulator> to IntelSimulator object"){
        std::unique_ptr<SimulatorGeneral<IntelSimulator> > s4(new IntelSimulator(8));
        REQUIRE(s4->getNumQubits() == 8);
    }
    SECTION("Create multiple simulator objects"){
        std::unique_ptr<ISimulator> s5(new IntelSimulator(8));
        std::unique_ptr<SimulatorGeneral<IntelSimulator>> s6(new IntelSimulator(8));
        REQUIRE(s5 != s6);
        REQUIRE(s5->getNumQubits() == 8);
        REQUIRE(s5->getNumQubits() == 8);
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
