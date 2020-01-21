/**
 * @file test_qft.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Tests for the quantum Fourier transform (QFT) implementation. 
 * @version 0.1
 * @date 2019-05-13
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
#include "qft.hpp"

using namespace QNLP;
#include <bitset>

/**
 * @brief Test: QFT forward 
 * 
 */
TEST_CASE("QFT forward","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);
    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / num_qubits ) ) );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx+1, max_idx);
        REQUIRE( r.GetProbability(min_idx) == 0.0 );
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / (num_qubits-1) ) ) );
        }
    }
}

/**
 * @brief Test: QFT inverse 
 * 
 */
TEST_CASE("QFT inverse","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);
    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyIQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / num_qubits ) ) );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyIQFT(min_idx+1, max_idx);
        REQUIRE( r.GetProbability(min_idx) == 0.0 );
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / (num_qubits-1) ) ) );
        }
    }
}

/**
 * @brief Test: QFT forward and inverse 
 * 
 */
TEST_CASE("QFT forward and inverse","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);

    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx, max_idx);
        sim.applyIQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx+1, max_idx);
        sim.applyIQFT(min_idx+1, max_idx);
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
    }
}