//#define CATCH_CONFIG_RUNNER

#include "ncu.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class NCU<IntelSimulator>;

TEST_CASE("Test n-controlled unitary module","[ncu]"){
    for(std::size_t num_qubits=4; num_qubits <=4; num_qubits++){
        std::size_t c_start = 0, c_end = num_qubits-2, target = num_qubits-1;
        DYNAMIC_SECTION("Testing " << num_qubits << " qubits"){
            IntelSimulator sim(num_qubits);
            REQUIRE(sim.getNumQubits() == num_qubits);
            std::size_t pattern_total = 1<<(num_qubits-1);
            for(std::size_t pattern_idx = 0; pattern_idx < pattern_total; pattern_idx++ ){
                sim.initRegister();
                DYNAMIC_SECTION("Testing pattern " << pattern_idx){
                    for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit++){
                        unsigned int window_select = (pattern_idx >> ctrl_qubit) & 0b1;
                        if(window_select == 1){
                            sim.applyGateX(ctrl_qubit);
                        }
                    }
                    NCU<decltype(sim)> nqd(sim.getGateX(), num_qubits-1);
                    nqd.applyNQubitControl(sim, c_start, c_end, target, sim.getGateX(), 0, true); //isX = true

                    if(pattern_idx != pattern_total-1){
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(0.0).margin(1e-12));
                    }
                    else{
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(1.0).margin(1e-12));
                    }
                }
            }
        }
    }
}
/*
TEST_CASE("Test n-controlled unitary simulator methods with Pauli-X","[ncu]"){
    for(std::size_t num_qubits=3; num_qubits <=8; num_qubits++){
        std::size_t c_start = 0, c_end = num_qubits-2, target = num_qubits-1;
        DYNAMIC_SECTION("Testing " << num_qubits << " qubits"){
            IntelSimulator sim(num_qubits);
            REQUIRE(sim.getNumQubits() == num_qubits);
            std::size_t pattern_total = 1<<(num_qubits-1);
            for(std::size_t pattern_idx = 0; pattern_idx < pattern_total; pattern_idx++ ){
                sim.initRegister();
                DYNAMIC_SECTION("Testing pattern " << pattern_idx){
                    for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit++){
                        unsigned int window_select = (pattern_idx >> ctrl_qubit) & 0b1;
                        if(window_select == 1){
                            sim.applyGateX(ctrl_qubit);
                        }
                    }
                    sim.applyGateNCU<decltype(sim.getGateX())>(sim.getGateX(), c_start, c_end, target);
                    if(pattern_idx != pattern_total-1){
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(0.0).margin(1e-12));
                    }
                    else{
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(1.0).margin(1e-12));
                    }
                }
            }
        }
    }
}*/
/* 
TEST_CASE("Test n-controlled unitary simulator methods with Pauli-Z","[ncu]"){
    int num_qubits = 5;
    IntelSimulator sim(num_qubits);
    auto &r = sim.getQubitRegister();

    sim.applyGateX(0);
    sim.applyGateX(1);
    sim.applyGateX(2);
    sim.applyGateX(3);

    sim.applyGateX(4);

    CAPTURE(r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9],r[10],r[11],r[12],r[13],r[14],r[15],r[16],r[17],r[18],r[19],r[20],r[21],r[22],r[23],r[24],r[25],r[26],r[27],r[28],r[29],r[30],r[31],"");

    sim.applyGateNCU<decltype(sim.getGateZ())>(sim.getGateZ(), 0, 3, 4);

    CAPTURE(r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9],r[10],r[11],r[12],r[13],r[14],r[15],r[16],r[17],r[18],r[19],r[20],r[21],r[22],r[23],r[24],r[25],r[26],r[27],r[28],r[29],r[30],r[31],"");
    REQUIRE(1 == 0);
}
*/