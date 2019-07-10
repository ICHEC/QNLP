#include "oracle.hpp"

#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;
template class Oracle<IntelSimulator>;

TEST_CASE("3 qubit oracles","[oracle]"){
    std::size_t num_qubits = 3;
    const std::vector<std::size_t> bit_patterns {0, 1, 2, 3};
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);
    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            sim.applyGateH(0);
            sim.applyGateH(1);
            sim.applyGateH(2);
            auto& r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], "");
            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());

            r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
            REQUIRE(r[i+4] != r[0]); //Not safest way to test for negation of the required bit
        }
    }
}

TEST_CASE("4 qubit oracles","[oracle]"){
    std::size_t num_qubits = 4;
    const std::vector<std::size_t> bit_patterns {0, 1, 2, 3, 4, 5, 6, 7};
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);
    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            sim.applyGateH(0);
            sim.applyGateH(1);
            sim.applyGateH(2);
            sim.applyGateH(3);

            auto& r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15], "");
            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());

            r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15], "");
            REQUIRE(r[i + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
            for(std::size_t j = 0; j < bit_patterns.size(); j++){
                if(j==i)
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
                else
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() > 0);
            }
        }
    }
}

TEST_CASE("5 qubit oracles","[oracle]"){
    std::size_t num_qubits = 5;
    std::vector<std::size_t> bit_patterns;
    for(std::size_t i = 0; i < pow(2,num_qubits-1); i++ ){
        bit_patterns.push_back(i);
    }
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }
    REQUIRE(ctrl_indices.size() == num_qubits-1);
    REQUIRE(ctrl_indices[0] == 0);
    REQUIRE(ctrl_indices[1] == 1);
    REQUIRE(ctrl_indices[2] == 2);
    REQUIRE(ctrl_indices[3] == 3);

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);
    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            for(int gate_idx = 0; gate_idx < num_qubits; gate_idx++){
                sim.applyGateX(gate_idx);
            }
            
            auto& r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15],
                    r[16],r[17],r[18], r[19], r[20], r[21], r[22], r[23],
                    r[24],r[25],r[26], r[27], r[28], r[29], r[30], r[31], "");

            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15],
                    r[16],r[17],r[18], r[19], r[20], r[21], r[22], r[23],
                    r[24],r[25],r[26], r[27], r[28], r[29], r[30], r[31], "");

            REQUIRE(r[i + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
            for(std::size_t j = 0; j < bit_patterns.size(); j++){
                if(j==i)
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
                else
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() > 0);
            }
        }
    }
}
