/**
 * @file test_arithmetic.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Tests for quantum arithmetic using the (QFT) implementation. 
 * @version 0.1
 * @date 2019-09-10
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
#include "bit_group.hpp"

using namespace QNLP;
#include <bitset>

/**
 * @brief Test BitGroup
 * 
 */
TEST_CASE("Test bit grouping","[bitgroup]"){
    std::size_t num_qubits = 8;
    std::vector<std::size_t> reg, aux;
    for (int i = 0; i < num_qubits; i++){
        if(i < num_qubits-2){ reg.push_back(i); }
        else{ aux.push_back(i); }
    }
    std::cout << reg.size() << "    | "<< aux.size() << std::endl;
    IntelSimulator sim(num_qubits);
    BitGroup<decltype(sim)> bg;
    auto& r = sim.getQubitRegister();
    /*
    SECTION("Group to right |010100>|10> -> |000011>|10>"){
        sim.initRegister();
        sim.applyGateX(reg[1]);
        sim.applyGateX(reg[3]);

        sim.applyGateX(aux[0]);
        bg.bit_swap_s2e(sim, reg, aux);

        sim.PrintStates("Post");
    }*/
    SECTION("Group to right |010000>|10> + |011000>|10> -> |000001>|10> + |000011>|10>"){
        sim.initRegister();
        sim.applyGateX(reg[1]);
        sim.applyGateH(reg[2]);

        sim.applyGateX(aux[0]);
        bg.bit_group(sim, reg, aux, true);

        sim.PrintStates("PostSuper");
    }

}

//        1.00000000    + i * 0.00000000      % |100110> p=1.000000
//        1.00000000    + i * 0.00000000      % |110010> p=1.000000