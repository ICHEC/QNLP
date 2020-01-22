//#define CATCH_CONFIG_RUNNER

#include "ncu.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

typedef ComplexDP Type;

/**
 * @brief Test PauliX for NCU
 * 
 */
TEST_CASE("Test n-controlled unitary module with Pauli-X |11..10>","[ncu]"){
    std::size_t max_qubits = 8;
    double mach_eps = 7./3. - 4./3. -1.;
    std::vector<std::size_t> ctrl_lines;
    //Generate simulator with n-qubits
    for(std::size_t num_qubits = 4; num_qubits <= max_qubits; num_qubits++){
        ctrl_lines.clear();
        for(std::size_t i = 0; i <= num_qubits-2; i++){
            ctrl_lines.push_back(i);    
        }
        DYNAMIC_SECTION("Testing " << num_qubits << " qubits"){

            IntelSimulator sim(num_qubits);

            // Loop over all possible control patterns
            std::size_t max_pattern = (int) pow(2, num_qubits-1);
            for(std::size_t pattern = 0; pattern < max_pattern; pattern++ ){
                DYNAMIC_SECTION("Testing pattern " << pattern ){
                    sim.initRegister();

                    // Loop over each individual qubit
                    for(std::size_t control_qubit_idx = 0; control_qubit_idx < num_qubits-1; control_qubit_idx++){
                        // Check if pattern has bit set
                        unsigned int window_select = (pattern >> control_qubit_idx) & 0b1;
                        if(window_select == 1){
                            sim.applyGateX(control_qubit_idx);
                        }
                    }

                    auto &r = sim.getQubitRegister();
                    sim.applyGateNCU(sim.getGateX(), ctrl_lines, num_qubits-1, "X");

                    if(pattern == max_pattern -1){
                        CHECK(sim.getQubitRegister().GetProbability(num_qubits-1) == Approx(1.).margin(1e-12));
                    }
                    else{
                        CHECK(sim.getQubitRegister().GetProbability(num_qubits-1) == Approx(0.).margin(1e-12));
                    }
                }
            }
        }
    }
}

/**
 * @brief Test Pauli-Z for NCU
 * 
 */
TEST_CASE("Test n-controlled unitary module with Pauli-Z on |111..1>","[ncu]"){
    std::size_t max_qubits = 8;
    double mach_eps = 7./3. - 4./3. -1.;

    std::vector<std::size_t> ctrl_lines;
    //Generate simulator with n-qubits
    for(std::size_t num_qubits = 4; num_qubits <= max_qubits; num_qubits++){
        ctrl_lines.clear();
        for(std::size_t i = 0; i <= num_qubits-2; i++){
            ctrl_lines.push_back(i);    
        }
        DYNAMIC_SECTION("Testing " << num_qubits << " qubits"){
            IntelSimulator sim(num_qubits);
            // Loop over all possible control patterns
            std::size_t num_patterns = (int) pow(2, num_qubits);

            for(std::size_t pattern = 0; pattern < num_patterns; pattern++ ){
                DYNAMIC_SECTION("Testing pattern " << pattern ){
                    sim.initRegister();
                    // Loop over each individual qubit
                    for(std::size_t control_qubit_idx = 0; control_qubit_idx < num_qubits; control_qubit_idx++){
                        // Check if pattern has bit set
                        unsigned int window_select = (pattern >> control_qubit_idx) & 0b1;
                        if(window_select == 1){
                            sim.applyGateX(control_qubit_idx);
                        }
                    }
                    auto &r = sim.getQubitRegister();
                    sim.applyGateNCU(sim.getGateZ(), ctrl_lines, num_qubits-1, "Z");

                    // The pattern matching |11..1>|1> will flip the sign of the last qubit. Others will remain unaffected.
                    if(pattern == num_patterns-1){
                        CAPTURE(pattern, r[pattern]);
                        CHECK(r[pattern].real()  == Approx(-1.).margin(1e-12) );
                        CHECK(r[pattern].imag() + 10*mach_eps == Approx(0.).margin(1e-12) ); //mach_eps to overcome floating point 0.
                    }
                    else{
                        CAPTURE(pattern, r[pattern]);
                        CHECK(r[pattern].real() == Approx(1.).margin(1e-12) );
                        CHECK(r[pattern].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
                    }
                }
            }
        }
    } 
}

/**
 * @brief Test Pauli-X and non-adjacent controls for NCU
 * 
 */
TEST_CASE("Test n-controlled unitary module with Pauli-X and non-adjacent controls","[ncu]"){
    const std::size_t max_qubits = 8;

    for(std::size_t num_qubits = 2; num_qubits <= max_qubits; num_qubits++ ){
        IntelSimulator sim(num_qubits);

        //Smallest use case
        if(num_qubits == 3){
            std::vector<std::size_t> vals0 {0,1}, vals1 {0,2}, vals2 {1,2};
            SECTION("3 qubit test"){
                for(std::size_t state = 0; state < (std::size_t) (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1} STATE VALUE=" << state){
                        sim.initRegister();

                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        CAPTURE(state);
                        CAPTURE(vals0);

                        sim.applyGateNCU(sim.getGateX(), vals0, 2, "X");
                        vals0.push_back(2);
                        if(state == (std::size_t) (0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals0, true) == 7);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals0, true) != 7);
                    }
                    DYNAMIC_SECTION("CTRLs {0,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 1, "X");

                        vals1.push_back(1);
                        CAPTURE(state, (0b1<<(num_qubits-1)) - 1, state == (std::size_t)(0b1<<(num_qubits-1)) - 1);

                        if(state == (std::size_t) (0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals1, true) == 7);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals1, true) != 7);
                    }
                    DYNAMIC_SECTION("CTRLs {1,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals2[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals2, 0, "X");
                        vals2.push_back(0);
                        if(state == (std::size_t) (0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals2, true) == 7);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals2, true) != 7);
                    }
                }
            }
        }
        //Even use case
        else if(num_qubits == 4){
            std::vector<std::size_t> vals0 {0,1,2}, vals1 {0,1,3}, vals2 {0,2};
            SECTION("4 qubit test"){
                for(std::size_t state = 0; state < (std::size_t) (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals0, 3, "X");
                        vals0.push_back(3);
                        if(state == (std::size_t)(0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals0, true) == 15);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals0, true) != 15);
                    }
                    DYNAMIC_SECTION("CTRLs {0,1,3} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 2, "X");
                        vals1.push_back(2);

                        if(state == (std::size_t) (0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals1, true) == 15);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals1, true) != 15);
                    }
                }
            }
        }

        //Non-trivial odd use case
        else if(num_qubits == 5){
            std::vector<std::size_t> vals0 {0,1,2,3}, vals1 {0,1,3,4};
            SECTION("5 qubit test"){
                for(std::size_t state = 0; state < (std::size_t) (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1,2,3} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals0, 4, "X");
                        vals0.push_back(4);
                        if(state == (std::size_t)(0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals0, true) == 31);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals0, true) != 31);
                    }
                    DYNAMIC_SECTION("CTRLs {0,1,3,4} STATE VALUE=" << state){
                        sim.initRegister();
                        for(std::size_t ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 2, "X");
                        vals1.push_back(2);

                        if(state == (std::size_t) (0b1<<(num_qubits-1)) - 1)
                            CHECK(sim.applyMeasurementToRegister(vals1, true) == 31);
                        else                            
                            CHECK(sim.applyMeasurementToRegister(vals1, true) != 31);
                    }
                }
            }
        }
    }
}