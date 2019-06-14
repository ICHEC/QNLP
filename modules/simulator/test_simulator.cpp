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
//#define CATCH_CONFIG_RUNNER
//#define CATCH_CONFIG_MAIN

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


TEST_CASE("Measurement of qubits"){
    std::size_t num_qubits = 3;

    SECTION("Measure single qubit"){

        SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(num_qubits);

        SECTION("State |0>"){
            REQUIRE(sim->applyMeasurement(0) == 0 );
        }
        SECTION("State |1>"){
            sim->applyGateX(0);
            REQUIRE(sim->applyMeasurement(0) == 1 );
        }
    }

    // Only one encoded state so there is a 100% certainty
    // of the measured output.
    SECTION("Measure multiple qubits"){

        std::size_t test_val = 0;

        std::vector<std::size_t> reg(num_qubits);
        for(std::size_t i = 0; i < num_qubits; i++){
            reg[i] = i;
        }
        for(std::size_t i = 0; i < num_qubits + 1; i++){
            DYNAMIC_SECTION("Encoded " << i){ 
                SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(num_qubits);

                for(std::size_t j = 0; j < i; j++){
                    sim->applyGateX(reg[j]);

                    test_val = (test_val << 1) | 1;
                }

                REQUIRE(sim->applyMeasurementToRegister(reg) == test_val);
            }
        }
    }
}




TEST_CASE("Encoding even distribution: Unique Binary Patterns"){
    
    /*  // Not included as CHECK_THROWS does not work with assert()
    SECTION("Check pattern passed is unique assert works"){
        std::size_t num_qubits_mem = 3;
        std::vector<std::size_t> reg_mem(num_qubits_mem);
        for(std::size_t i = 0; i < num_qubits_mem; i++){
            reg_mem[i] = i;
        }
        std::vector<std::size_t> reg_ancilla(num_qubits_mem+2);
        for(std::size_t i = 0; i < num_qubits_mem + 2; i++){
            reg_ancilla[i] = i + num_qubits_mem;
        }

        std::size_t num_bin_patterns = 4; 

        std::vector<std::size_t> bin_patterns(num_bin_patterns);
        bin_patterns[0] = 1;
        bin_patterns[1] = 2;
        bin_patterns[2] = 1;
        bin_patterns[3] = 3;

        SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(2*num_qubits_mem + 2);

        CHECK_THROWS(sim->encodeBinToSuperpos_unique(reg_mem, reg_ancilla, bin_patterns,num_qubits_mem));
    }
    */

    SECTION("Check encoding works correctly without throws and with expected distribution"){
        std::size_t max_num_qubits_mem = 6;
        std::size_t num_exp = 500;

        std::size_t result;


        // Repeat test for varying numbers of qubits
        for( std::size_t num_qubits_mem = 3; num_qubits_mem < max_num_qubits_mem; num_qubits_mem++){
            DYNAMIC_SECTION("Encoding " << num_qubits_mem << " qubits"){

                std::vector<std::size_t> reg_mem(num_qubits_mem);
                for(std::size_t i = 0; i < num_qubits_mem; i++){
                    reg_mem[i] = i;
                }
                std::vector<std::size_t> reg_ancilla(num_qubits_mem+2);
                for(std::size_t i = 0; i < num_qubits_mem + 2; i++){
                    reg_ancilla[i] = i + num_qubits_mem;
                }

                std::size_t num_bin_patterns = 4; 

                std::vector<std::size_t> bin_patterns(num_bin_patterns);
                bin_patterns[0] = pow(2,num_qubits_mem)-1;
                bin_patterns[1] = 1;
                bin_patterns[2] = (std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2);
                bin_patterns[3] = ((std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2)) << (num_qubits_mem / 2);

                std::map<std::size_t, std::size_t> count_bin_pattern;
                for(std::size_t i = 0; i < num_bin_patterns; i++){
                    count_bin_pattern.insert(std::pair<std::size_t,std::size_t>(bin_patterns[i],0));
                }

                SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(2*num_qubits_mem + 2);

                for(std::size_t exp = 0; exp < num_exp; exp++){
                    
                    sim->initRegister();
                    sim->encodeBinToSuperpos_unique(reg_mem, reg_ancilla, bin_patterns,num_qubits_mem);
                    result = sim->applyMeasurementToRegister(reg_mem);

                    // Check measured result is valid
                    CHECK_THAT(bin_patterns, Catch::Matchers::VectorContains(result));

                    // Update distributions of the results
                    count_bin_pattern[result]++;
                }

                // Check resulting distribution of results lies within an acceptable limit.
                // For smaller sample sizes ~500, 30% epsilon value is qualitatively said to be an acceptable limit.
                Approx target = Approx(num_exp / num_bin_patterns).epsilon(0.30);
                for(std::map<std::size_t,std::size_t>::iterator it = count_bin_pattern.begin(); it != count_bin_pattern.end(); it++){
                    CHECK( target == (double) it->second);
                }
                
            }

        }
    }
}


/*  // encodBinTo Superpos is deprecated due to issue with algorithm. Thus only the encode..._unique is valid.
TEST_CASE("Encoding even distribution: Binary"){

    //  // Not included as CHECK_THROWS does not work with assert()
    SECTION("Check pattern passed is does not contain a zero assert works"){
        std::vector<std::size_t> reg_mem(num_qubits_mem);
        for(std::size_t i = 0; i < num_qubits_mem; i++){
            reg_mem[i] = i;
        }
        std::vector<std::size_t> reg_ancilla(num_qubits_mem+2);
        for(std::size_t i = 0; i < num_qubits_mem + 2; i++){
            reg_ancilla[i] = i + num_qubits_mem;
        }

        std::size_t num_bin_patterns = 4; 

        std::vector<std::size_t> bin_patterns(num_bin_patterns);
        bin_patterns[0] = 1;
        bin_patterns[1] = 0;
        bin_patterns[2] = 1;//(std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2);
        bin_patterns[3] = 3;//((std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2)) << (num_qubits_mem / 2);

        SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(2*num_qubits_mem + 2);

        CHECK_THROWS(sim->encodeBinToSuperpos(reg_mem, reg_ancilla, bin_patterns,num_qubits_mem));
    }
    



    SECTION("Check encoding works correctly without throws and with expected distribution"){
        std::size_t max_num_qubits_mem = 6;
        std::size_t num_exp = 500;
        std::size_t result;

        // Repeat test for varying numbers of qubits
        for( std::size_t num_qubits_mem = 3; num_qubits_mem < max_num_qubits_mem; num_qubits_mem++){
            DYNAMIC_SECTION("Encoding " << num_qubits_mem << " qubits"){

                std::vector<std::size_t> reg_mem(num_qubits_mem);
                for(std::size_t i = 0; i < num_qubits_mem; i++){
                    reg_mem[i] = i;
                }
                std::vector<std::size_t> reg_ancilla(num_qubits_mem+2);
                for(std::size_t i = 0; i < num_qubits_mem + 2; i++){
                    reg_ancilla[i] = i + num_qubits_mem;
                }

                std::size_t num_bin_patterns = 3; 

                std::vector<std::size_t> bin_patterns(num_bin_patterns);
                //bin_patterns[0] = pow(2,num_qubits_mem)-4;
                //bin_patterns[1] = 1;
                //bin_patterns[2] = (std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2);
                //bin_patterns[3] = ((std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2)) << (num_qubits_mem / 2);
                bin_patterns[0] = 2;
                bin_patterns[1] = 1;
                bin_patterns[2] = 1;//(std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2);
                //bin_patterns[3] = 4;//((std::size_t)pow(2,num_qubits_mem)-1 >> (num_qubits_mem / 2)) << (num_qubits_mem / 2);



                std::map<std::size_t, std::size_t> count_bin_pattern;
                for(std::size_t i = 0; i < num_bin_patterns; i++){
                    count_bin_pattern.insert(std::pair<std::size_t,std::size_t>(bin_patterns[i],0));
                }



                SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(2*num_qubits_mem + 2);

                for(std::size_t exp = 0; exp < num_exp; exp++){
                    
                    sim->initRegister();
                    sim->encodeBinToSuperpos(reg_mem, reg_ancilla, bin_patterns,num_qubits_mem);
                    result = sim->applyMeasurementToRegister(reg_mem);

                    // Check measured result is valid
                    CHECK_THAT(bin_patterns, Catch::Matchers::VectorContains(result));

                    // Update distributions of the results
                    count_bin_pattern[result]++;
                }

                // Check resulting distribution of results lies within an acceptable limit.
                // For smaller sample sizes ~500, 30% epsilon value is qualitatively said to be an acceptable limit.
                Approx target = Approx(num_exp / num_bin_patterns).epsilon(0.30);
                for(std::map<std::size_t,std::size_t>::iterator it = count_bin_pattern.begin(); it != count_bin_pattern.end(); it++){
                    CHECK( target == (double) it->second);
                }
                
            }

        }

    }
}
*/

/**
 * @brief User defined main required for this instance, as openqu::mpi::Environment destructor calls MPI_Finalize.
 * As we require MPI until the end of the test session, it must be created before the tests, and destroyed after
 * all have finished.
 */
/*
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
*/
