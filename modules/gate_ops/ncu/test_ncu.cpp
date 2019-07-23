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