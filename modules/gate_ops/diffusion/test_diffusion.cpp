#include "diffusion.hpp"
#include "oracle.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class NCU<IntelSimulator>;

TEST_CASE("4 qubit diffusion ","[diffusion]"){
    std::size_t num_qubits = 4;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();

    SECTION("2^4 bit patterns (16)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for(std::size_t s = 0; s < (0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        //Create oracle object with num_ctrl_gates and indices
        Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);

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
                for(int ii = 1 ; ii < sqrt( (0b1<<num_qubits) ); ii++){
                    // Mark state: convert matching state pattern to |11...1>
                    // apply nCZ, and undo conversion; negates the matched pattern phase
                    oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());

                    CAPTURE( reg[i], i );
                    //REQUIRE( reg[i].real() < 0.);
                    reg.Print("PRE-DIFF iteration=" + std::to_string(ii));

                    diffusion.applyOpDiffusion( sim, 0, 2, 3);
                    CAPTURE( reg[i], i );

                    reg.Print("POST-DIFF iteration=" + std::to_string(ii));
                }
            }
        }
    }
}