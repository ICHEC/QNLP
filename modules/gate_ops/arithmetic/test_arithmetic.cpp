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
#include "arithmetic.hpp"

using namespace QNLP;
#include <bitset>

/**
 * @brief Test Arithmetic: subtraction
 * 
 */
TEST_CASE("Arithmetic subtraction","[arithmetic]"){
    std::size_t num_qubits = 6;
    IntelSimulator sim(num_qubits);
    Arithmetic<decltype(sim)> arr;
    auto& r = sim.getQubitRegister();
    
    SECTION("OLD_TEST"){
        unsigned int r1_min = 0, r1_max=(num_qubits/2)-1, r2_min = r1_max+1, r2_max = num_qubits-1;
        CAPTURE(r1_min, r1_max, r2_min, r2_max );

        for(std::size_t pattern = 0; pattern < 1<<(num_qubits/2); pattern ++){
            unsigned int bitmask = 0x1;
            unsigned int a = 0;
            sim.initRegister();
            for (unsigned int i=0; i <= r1_max -1; i++){
                sim.applyGateX(i);
                a += (unsigned int) pow(2,i);
            }
            for (unsigned int j = r2_min; j <= r2_max; j++){
                if ( (pattern & bitmask) > 0 ){
                    sim.applyGateX(j);
                }
                bitmask <<=1;
            }
            CAPTURE(pattern, a, bitmask);
            arr.sub_reg(sim, r1_min, r1_max, r2_min, r2_max);
            std::string result="";
            CAPTURE(result);

            for (unsigned int j = r2_min; j <= r2_max; j++){
                result += std::to_string( r.GetProbability( j )).at(0);
            }
            CAPTURE(result);

            std::reverse(result.begin(), result.end());
            std::cout << "Sub = " << std::bitset<4>(a) << "-" << std::bitset<4>(pattern) << "=> Expected:=" << std::bitset<4>(a-pattern) << " : Actual:=" << result << std::endl;
            CAPTURE(r);
        }
    }
}

/**
 * @brief Test Arithmetic: Summation
 * 
 */
TEST_CASE("Arithmetic summation","[arithmetic]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);
    Arithmetic<decltype(sim)> arr;
    auto& r = sim.getQubitRegister();
    
    SECTION("OLD_TEST","[arithmetic]"){
        unsigned int r1_min = 0, r1_max=(num_qubits/2)-1, r2_min = r1_max+1, r2_max = num_qubits-1;
        for(int pattern = 0; pattern < 1<<(num_qubits/2); pattern ++){
            unsigned int bitmask = 0x1;
            unsigned int a = 0;
            sim.initRegister();
            for (unsigned int i=0; i <= r1_max -1; i++){
                sim.applyGateX(i);
                a += (unsigned int) pow(2,i);
            }
            for (unsigned int j = r2_min; j <= r2_max; j++){
                if ( (pattern & bitmask) > 0 ){
                    sim.applyGateX(j);
                }
                bitmask <<=1;
            }
            arr.sum_reg(sim, r1_min, r1_max, r2_min, r2_max);
            std::string result="";

            for (unsigned int j = r2_min; j <= r2_max; j++){
                result += std::to_string( r.GetProbability( j )).at(0);
            }
            std::reverse(result.begin(), result.end());
            std::cout << "Sub = " << std::bitset<4>(a) << "-" << std::bitset<4>(pattern) << "=> Expected:=" << std::bitset<4>(a-pattern) << " : Actual:=" << result << std::endl;
            CAPTURE(r);
        }
    }
}
