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
/*
TEST_CASE("Test n-controlled unitary module with Pauli-X |11..10>","[ncu]"){
    std::size_t max_qubits = 4;
    double mach_eps = 7./3. - 4./3. -1.;

    //Generate simulator with n-qubits
    for(std::size_t num_qubits = 4; num_qubits <= max_qubits; num_qubits++){
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
                    sim.applyGateNCU(sim.getGateX(), 0, num_qubits-2, num_qubits-1);
                    
                    if(pattern == max_pattern -1){
                        REQUIRE(sim.getQubitRegister().GetProbability(num_qubits-1) == Approx(1.).margin(1e-12));
                    }
                    else{
                        REQUIRE(sim.getQubitRegister().GetProbability(num_qubits-1) == Approx(0.).margin(1e-12));
                    }
                }
            }
        }
    } 
}

TEST_CASE("Test n-controlled unitary module with Pauli-Z on |111..1>","[ncu]"){
    std::size_t max_qubits = 8;
    double mach_eps = 7./3. - 4./3. -1.;

    //Generate simulator with n-qubits
    for(std::size_t num_qubits = 4; num_qubits <= max_qubits; num_qubits++){
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
                    sim.applyGateNCU(sim.getGateZ(), 0, num_qubits-2, num_qubits-1);

                    // The pattern matching |11..1>|1> will flip the sign of the last qubit. Others will remain unaffected.
                    if(pattern == num_patterns-1){
                        CAPTURE(pattern, r[pattern]);
                        REQUIRE(r[pattern].real()  == Approx(-1.).margin(1e-12) );
                        REQUIRE(r[pattern].imag() + 10*mach_eps == Approx(0.).margin(1e-12) ); //mach_eps to overcome floating point 0.
                    }
                    else{
                        CAPTURE(pattern, r[pattern]);
                        REQUIRE(r[pattern].real() == Approx(1.).margin(1e-12) );
                        REQUIRE(r[pattern].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
                    }
                }
            }
        }
    } 
}
*/

TEST_CASE("Test n-controlled unitary module with Pauli-X and non-adjacent controls","[ncu]"){
    const std::size_t max_qubits = 5;
/*
    sim.PrintStates("Pre-ncu");
    sim.applyGateNCU(sim.getGateX(), ctrl0, 2);
    //sim.applyGateNCU(sim.getGateX(), ctrl1, 5);
    sim.PrintStates("Post-ncu");
*/
    for(std::size_t num_qubits = 2; num_qubits <= max_qubits; num_qubits++ ){
        IntelSimulator sim(num_qubits);

        //Smallest use case
        if(num_qubits == 3){
            std::vector<std::size_t> vals0 {0,1}, vals1 {0,2}, vals2 {1,2};
            SECTION("3 qubit test"){
                for(int state = 0; state < (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals0, 2);
                        vals0.push_back(2);
                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) == 7);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) != 7);
                    }
                    DYNAMIC_SECTION("CTRLs {0,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 1);

                        vals1.push_back(1);
                        CAPTURE(state, (0b1<<(num_qubits-1)) - 1, state == (0b1<<(num_qubits-1)) - 1);

                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) == 7);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) != 7);
                    }
                    DYNAMIC_SECTION("CTRLs {1,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals2[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals2, 0);
                        vals2.push_back(0);
                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) == 7);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) != 7);
                    }
                }
            }
        }
        //Even use case
        else if(num_qubits == 4){
            std::vector<std::size_t> vals0 {0,1,2}, vals1 {0,1,3}, vals2 {0,2};
            SECTION("4 qubit test"){
                for(int state = 0; state < (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1,2} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals0, 3);
                        vals0.push_back(3);
                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) == 15);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) != 15);
                    }
                    DYNAMIC_SECTION("CTRLs {0,1,3} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 2);
                        vals1.push_back(2);

                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) == 15);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) != 15);
                    }
                    /* // Non unqiue values makes this tricky to test cleanly
                    DYNAMIC_SECTION("CTRLs {0,2} STATE VALUE=" << state << " TARGET=1"){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < vals2.size(); ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals2[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals2, 1);
                        vals2.push_back(1);
                        if(state == 7)
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) == 7);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) != 7);
                    }

                    DYNAMIC_SECTION("CTRLs {0,2} STATE VALUE=" << state << " TARGET=3"){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < vals2.size(); ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals2[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals2, 3);
                        vals2.push_back(3);
                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) == 13);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals2, true) != 13);
                    }
                    */
                }
            }


        }

        //Non-trivial odd use case
        else if(num_qubits == 5){
            std::vector<std::size_t> vals0 {0,1,2,3}, vals1 {0,1,3,4};
            SECTION("5 qubit test"){
                for(int state = 0; state < (0b1<<(num_qubits-1)); state++){
                    DYNAMIC_SECTION("CTRLs {0,1,2,3} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals0[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals0, 4);
                        vals0.push_back(4);
                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) == 31);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals0, true) != 31);
                    }
                    DYNAMIC_SECTION("CTRLs {0,1,3,4} STATE VALUE=" << state){
                        sim.initRegister();
                        for(int ctrl_idx = 0; ctrl_idx < num_qubits; ctrl_idx++){
                            unsigned int window_select = (state >> ctrl_idx) & 0b1;
                            if(window_select == 1){
                                sim.applyGateX(vals1[ctrl_idx]);
                            }
                        }
                        sim.applyGateNCU(sim.getGateX(), vals1, 2);
                        vals1.push_back(2);

                        if(state == (0b1<<(num_qubits-1)) - 1)
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) == 31);
                        else                            
                            REQUIRE(sim.applyMeasurementToRegister(vals1, true) != 31);
                    }
                }
            }
        }
    }
}