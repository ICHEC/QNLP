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

using namespace QNLP;

/**
 * @brief Tests creating a simulator (in this case, the Intel-QS), for a variety of different qubit counts, up to a max limit.
 * 
 */
TEST_CASE("Testing Intel-QS simulator creation","[simulator]"){
    std::string label = "";
    for(std::size_t num_qubits = 0; num_qubits<16; num_qubits++){
        label = "Testing " + std::to_string(num_qubits) + " qubits";
        SECTION(label){
            IntelSimulator sim_test(num_qubits);
        }
    }
}

TEST_CASE("Testing Intel-QS simulator","[simulator]"){
    
}
