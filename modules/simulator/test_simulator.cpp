/**
 * @file test_simulator.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief 
 * @version 0.1
 * @date 2019-05-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "catch2/catch.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "sim_factory.cpp"

using namespace QNLP;

/**
 * @brief Tests creating a simulator (in this case, the Intel-QS), for a variety of different qubit counts, up to a max limit.
 * 
 */
TEST_CASE("Testing Intel-QS simulator creation with 'new'","[simulator]"){
    Simulator *sim_test = nullptr;
    std::string label = "";
    for(std::size_t num_qubits = 0; num_qubits<16; num_qubits++){
        label = "Testing " + std::to_string(num_qubits) + " qubits";
        SECTION(label){
            REQUIRE(sim_test == nullptr);
            sim_test = new IntelSimulator(num_qubits);
            delete sim_test;
        }
        sim_test = nullptr;
    }
}

TEST_CASE("Testing Intel-QS simulator","[simulator]"){
    auto s = Simulator::createSimulator(0,8);
    
}
