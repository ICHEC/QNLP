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
#include "sim_interface.hpp"
#include "qhipster_interface.hpp"
#include "sim_factory.cpp"

using namespace QNLP;

/**
 * @brief Tests creating a simulator (in this case, the Intel-QS), for a variety of different qubit counts, up to a max limit.
 * 
 */
TEST_CASE("Testing Intel-QS simulator creation with 'new'","[simulator]"){
    Simulator *sim_test = nullptr;
    for(std::size_t num_qubits = 0; num_qubits<16; num_qubits++){
        SECTION("Testing " + string(num_qubits) + " qubits"){
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