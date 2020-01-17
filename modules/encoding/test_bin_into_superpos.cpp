//#define CATCH_CONFIG_RUNNER

#include "bin_into_superpos.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;
using namespace Catch::Matchers;

typedef ComplexDP Type;

template class QNLP::EncodeBinIntoSuperpos<IntelSimulator>;

TEST_CASE("Test encoding of binary (integers) to superposition","[encode]"){
    SECTION("Testing qubit encoding"){

        std::size_t num_qubits = 10;

        std::size_t len_reg_memory = (num_qubits - 2) / 2;
        std::size_t len_reg_auxiliary = len_reg_memory + 2;
        std::size_t num_bin_pattern = pow(2,len_reg_memory) - 2;

        IntelSimulator sim(num_qubits);
        sim.initRegister();

        REQUIRE(sim.getNumQubits() == num_qubits);

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
        for(std::size_t i = 2; i < num_bin_pattern; i++){
            vec_to_encode[i-2] = i;
        }

        std::size_t val;
        // Encode
        EncodeBinIntoSuperpos<decltype(sim)> encoder(num_bin_pattern, len_reg_memory);
        encoder.encodeBinInToSuperpos_unique(sim, reg_memory, reg_auxiliary, vec_to_encode);

        // Measure
        val = sim.applyMeasurementToRegister(reg_memory);

        CHECK_THAT(vec_to_encode, VectorContains(val));
    }
}

TEST_CASE("Test encoding of different register sizes and checking states' amplitudes","[encode_amp]"){
    const std::size_t max_qubits = 5;
    double mach_eps = 7./3. - 4./3. -1.;

    std::size_t num_qubits;
    std::size_t len_reg_auxiliary;
    std::size_t num_bin_pattern;
    std::size_t val;

    std::size_t test_pattern = 2;

    double expected_val;

    for(std::size_t len_reg_memory = 2; len_reg_memory < max_qubits; len_reg_memory++){
        DYNAMIC_SECTION("Testing " << len_reg_memory << " memory qubits"){

            // Experiment set-up
            num_qubits = 2*len_reg_memory + 2;
            len_reg_auxiliary = len_reg_memory + 2;
            num_bin_pattern = pow(2,len_reg_memory);

            // Expected real part of amplitude for equally encoded states
            expected_val = sqrt(1.0 / (double) num_bin_pattern);

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

            for(int i = 0; i < num_bin_pattern; i++){
                REQUIRE(r[i].real() + 10*mach_eps == Approx(expected_val).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );

            }
            for(int i = num_bin_pattern; i < pow(2,2*len_reg_memory); i++){
                REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
            }
            for(int i = pow(2,2*len_reg_memory); i < pow(2,2*len_reg_memory) + num_bin_pattern; i++){
                REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
            }
            for(int i = pow(2,2*len_reg_memory) + num_bin_pattern; i < pow(2,num_qubits); i++){
                REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
            }
        

            sim.collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1);
            val = sim.applyMeasurementToRegister(reg_memory);

            CHECK_THAT(vec_to_encode, VectorContains(val));

        }
    }
}
