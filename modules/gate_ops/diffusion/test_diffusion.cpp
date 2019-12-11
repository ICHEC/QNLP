#include "diffusion.hpp"
#include "oracle.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

//template class NCU<IntelSimulator>;

TEST_CASE("4 qubit diffusion using module","[diffusion]"){
    std::size_t num_qubits = 4;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    double previous_prob = 0.;

    SECTION("2^4 bit patterns (16)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for(std::size_t s = 0; s < (std::size_t)(0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        //Create oracle object with num_ctrl_gates and indices
        Oracle<decltype(sim)> oracle;

        Diffusion<decltype(sim)> diffusion;

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // sqrt(N) iterations optimal 
                for(int iteration = 1 ; iteration < (M_PI/4)*sqrt( (0b1<<num_qubits) / 1); iteration++){
                    // Mark state: convert matching state pattern to |11...1>
                    // apply nCZ, and undo conversion; negates the matched pattern phase
                    oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                    CAPTURE( reg[i], i );
                    //REQUIRE( reg[i].real() < 0.);
                    //reg.Print("PRE-DIFF iteration=" + std::to_string(iteration));

                    diffusion.applyOpDiffusion( sim, ctrl_indices, num_qubits-1);
                    CAPTURE( reg[i], i );
                    if(i>0){
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[0])*abs(reg[0]) );
                    }
                    else {
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[1])*abs(reg[1]) );
                    }
                    REQUIRE( abs(reg[i])*abs(reg[i]) > previous_prob );
                    previous_prob = abs(reg[i])*abs(reg[i]);
                }
            }
        }
    }
}


TEST_CASE("8 qubit diffusion using module","[diffusion]"){
    std::size_t num_qubits = 8;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    double previous_prob = 0.;

    SECTION("2^8 bit patterns (256)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for(std::size_t s = 0; s < (std::size_t)(0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        //Create oracle object with num_ctrl_gates and indices
        Oracle<decltype(sim)> oracle;

        Diffusion<decltype(sim)> diffusion;

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // O sqrt(N) iterations optimal 
                // R = (M_PI/4)*sqrt( (0b1<<num_qubits) / 1);
                for(int iteration = 1 ; iteration < (M_PI/4)*sqrt( (0b1<<num_qubits) / 1); iteration++){
                    // Mark state: convert matching state pattern to |11...1>
                    // apply nCZ, and undo conversion; negates the matched pattern phase
                    oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                    CAPTURE( reg[i], i );
                    //REQUIRE( reg[i].real() < 0.);
                    //reg.Print("PRE-DIFF iteration=" + std::to_string(iteration));

                    diffusion.applyOpDiffusion( sim, ctrl_indices, num_qubits-1);
                    CAPTURE( reg[i], i );
                    if(i>0){
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[0])*abs(reg[0]) );
                    }
                    else {
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[1])*abs(reg[1]) );
                    }
                    REQUIRE( abs(reg[i])*abs(reg[i]) > previous_prob );
                    previous_prob = abs(reg[i])*abs(reg[i]);
                }
            }
        }
    }
}


TEST_CASE("4 qubit diffusion using Simulator method","[diffusion]"){
    std::size_t num_qubits = 4;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    double previous_prob = 0.;

    SECTION("2^4 bit patterns (16)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for(std::size_t s = 0; s < (std::size_t)(0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // sqrt(N) iterations optimal 
                for(int iteration = 1 ; iteration < (M_PI/4)*sqrt( (0b1<<num_qubits) / 1); iteration++){
                    // Mark state: convert matching state pattern to |11...1>
                    // apply nCZ, and undo conversion; negates the matched pattern phase
                    sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                    CAPTURE( reg[i], i );
                    sim.applyDiffusion(ctrl_indices, num_qubits-1);

                    CAPTURE( reg[i], i );
                    if(i>0){
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[0])*abs(reg[0]) );
                    }
                    else {
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[1])*abs(reg[1]) );
                    }
                    REQUIRE( abs(reg[i])*abs(reg[i]) > previous_prob );
                    previous_prob = abs(reg[i])*abs(reg[i]);
                }
            }
        }
    }
}

TEST_CASE("8 qubit diffusion using Simulator method","[diffusion]"){
    std::size_t num_qubits = 8;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    double previous_prob = 0.;

    SECTION("2^8 bit patterns (256)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for(std::size_t s = 0; s < (std::size_t)(0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // O sqrt(N) iterations optimal 
                // R = (M_PI/4)*sqrt( (0b1<<num_qubits) / 1);
                for(int iteration = 1 ; iteration < (M_PI/4)*sqrt( (0b1<<num_qubits) / 1); iteration++){
                    // Mark state: convert matching state pattern to |11...1>
                    // apply nCZ, and undo conversion; negates the matched pattern phase
                    sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                    CAPTURE( reg[i], i );

                    sim.applyDiffusion(ctrl_indices, num_qubits-1);
                    CAPTURE( reg[i], i );
                    if(i>0){
                        
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[0])*abs(reg[0]) );
                    }
                    else {
                        REQUIRE( abs(reg[i])*abs(reg[i]) > abs(reg[1])*abs(reg[1]) );
                    }
                    REQUIRE( abs(reg[i])*abs(reg[i]) > previous_prob );
                    previous_prob = abs(reg[i])*abs(reg[i]);
                }
            }
        }
    }
}
