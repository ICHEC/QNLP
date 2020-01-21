//#define CATCH_CONFIG_RUNNER

#include "hamming_RotY_amplification.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

using namespace QNLP;
using namespace Catch::Matchers;

typedef ComplexDP Type;

template class QNLP::HammingDistanceRotY<IntelSimulator>;

/**
 * @brief Calulates the Hamming distance between two binary strings stored as integers
 * 
 * @param pattern1 First binary pattern
 * @param pattern2 Second binary pattern
 * @param len_bin_pattern Length if binary strings 
 * @return std::size_t Returns Hamming distance between binary representation of two integers.
 */
std::size_t calc_hammingDist(std::size_t pattern1, std::size_t pattern2, std::size_t len_bin_pattern){
    std::size_t num_diffs = 0;

    for(int i = 0; i < len_bin_pattern; i++){
        num_diffs += (IS_SET(pattern1,i) == IS_SET(pattern2,i));
    }
    return num_diffs;
}

/**
 * @brief Classically calculates the Hamming distance between each binary string in a vector and a test binary string 
 * 
 * @param target_vec Vector consisting of binary strings represented in std::size_t format 
 * @param test  Test vector for calculating Hamming distance between it and all binary strings in target_vec
 * @param len_bin_pattern Length if binary strings 
 * @return std::map<std::size_t, double> Returns Hash map of encoded integers and the real component of their expected amplitudes which is calculated classically. 
 */
std::map<std::size_t, double> expected_amplitude(std::vector<std::size_t>& target_vec, std::size_t test, std::size_t len_bin_pattern){
    const std::size_t num_bin_pattern = target_vec.size();
    double norm_factor = 0.0;

    // Calculate Hamming Distance for each training pattern to test pattern
    std::map<std::size_t, double> exp_amp; 
    for(std::size_t i = 0; i < num_bin_pattern; i++){
        exp_amp.insert(pair<std::size_t, double>(target_vec[i],(double) calc_hammingDist(target_vec[i],test, len_bin_pattern)));
    }

    for(auto& [k,v] : exp_amp){
        v = sin(v * 0.5* M_PI / (double) len_bin_pattern);
        norm_factor += v*v;
    }   

    norm_factor = sqrt(norm_factor);

    for(auto& [k,v] : exp_amp){
        v /= norm_factor;
    }

    return exp_amp;
}

/**
 * @brief Test the Hamming distance routine which uses Y rotations to encode the Hamming distance into each states' ampitudes. Each state's amplitude is calculated classically and compared to the quantum simulator computed counterpart.
 * 
 */
TEST_CASE("Test Hamming distance with Roatation about y axis routine","[hammingroty]"){
    const std::size_t max_qubits = 6;
    double mach_eps = 7./3. - 4./3. -1.;

    std::size_t num_qubits;
    std::size_t len_reg_auxiliary;
    std::size_t num_bin_pattern;
    std::size_t val;

    std::size_t test_pattern = 2;

    for(std::size_t len_reg_memory = 2; len_reg_memory < max_qubits; len_reg_memory++){
        DYNAMIC_SECTION("Testing " << len_reg_memory << " memory qubits"){

            for(std::size_t test_pattern = 2; test_pattern < pow(2,len_reg_memory); test_pattern++){
                DYNAMIC_SECTION("Testing test pattern (integer format) = " << test_pattern ){

                    // Experiment set-up
                    num_qubits = 2*len_reg_memory + 2;
                    len_reg_auxiliary = len_reg_memory + 2;
                    num_bin_pattern = pow(2,len_reg_memory);

                    IntelSimulator sim(num_qubits);
                    sim.initRegister();
                    auto &r = sim.getQubitRegister();

                    // Set up registers to store indices
                    std::vector<std::size_t> reg_memory(len_reg_memory);
                    for(std::size_t i = 0; i < len_reg_memory; i++){
                        reg_memory[i] = i;
                    }
                    std::vector<std::size_t> reg_auxiliary(len_reg_auxiliary);
                    for(std::size_t i = 0; i < len_reg_auxiliary; i++){
                        reg_auxiliary[i] = i + len_reg_memory;
                    }

                    // Init data to encode
                    std::vector<std::size_t> vec_to_encode(num_bin_pattern);
                    for(std::size_t i = 0; i < num_bin_pattern; i++){
                        vec_to_encode[i] = i;
                    }

                    // Encode
                    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory);

                    // Compute Hamming distance
                    sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_auxiliary, len_reg_memory, num_bin_pattern);

                    // Post-selection
                    sim.collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1);

                    // Generate expected amplitudes from classical computation
                    std::map<std::size_t, double> exp_amp =  expected_amplitude(vec_to_encode, test_pattern, len_reg_memory);

                    for(int i = 0; i < num_bin_pattern; i++){
                        REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                        REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );

                    }
                    for(int i = num_bin_pattern; i < pow(2,2*len_reg_memory); i++){
                        REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                        REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
                    }
                    // Check relevant state's amplitudes are set correctly.
                    for(int i = pow(2,2*len_reg_memory); i < pow(2,2*len_reg_memory) + num_bin_pattern; i++){

                        CAPTURE( i, r[i].real() + 10*mach_eps == Approx(exp_amp[vec_to_encode[i - pow(2,2*len_reg_memory)]]).margin(1e-12));
                        REQUIRE(r[i].real() + 10*mach_eps == Approx(exp_amp[vec_to_encode[i - pow(2,2*len_reg_memory)]]).margin(1e-12));
                        REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );

                    }
                    for(int i = pow(2,2*len_reg_memory) + num_bin_pattern; i < pow(2,num_qubits); i++){
                        REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                        REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );

                    }

                    val = sim.applyMeasurementToRegister(reg_memory);

                    CHECK_THAT(vec_to_encode, VectorContains(val));

                }
            }
        }
    }
}
