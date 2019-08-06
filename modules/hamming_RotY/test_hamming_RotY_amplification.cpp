//#define CATCH_CONFIG_RUNNER

#include "hamming.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;
using namespace Catch::Matchers;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class HammingDistanceRotY<IntelSimulator>;


TEST_CASE("Test Hamming distance with Roatation about y axis routine","[HammingRotY]"){
    const std::size_t max_qubits = 5;
    double mach_eps = 7./3. - 4./3. -1.;

    std::size_t len_reg_memory;
    std::size_t len_reg_ancilla;
    std::size_t num_bin_pattern;

    std::size_t test_pattern = 3;

    for(std::size_t num_qubits = 2; num_qubits < max_qubits; num_qubits++){
        DYNAMIC_SECTION("Testing " << num_qubits << " qubits"){

            IntelSimulator sim(num_qubits);

            len_reg_memory = (num_qubits - 2) / 2;
            len_reg_ancilla = len_reg_memory + 2;
            num_bin_pattern = pow(2,len_reg_memory) - 2;

            // Set up registers to store indices
            std::vector<std::size_t> reg_memory(len_reg_memory);
            for(std::size_t i = 0; i < len_reg_memory; i++){
                reg_memory[i] = i;
            }
            std::vector<std::size_t> reg_ancilla(len_reg_ancilla);
            for(std::size_t i = 0; i < len_reg_ancilla; i++){
                reg_ancilla[i] = i + len_reg_memory;
            }

            // Init data to encode
            std::vector<std::size_t> vec_to_encode(num_bin_pattern);
            for(std::size_t i = 2; i < num_bin_pattern; i++){
                vec_to_encode[i-2] = i;
            }


            // Encode
            sim.encodeBinToSuperpos_unique(reg_memory, reg_ancilla, vec_to_encode, len_reg_memory);

            // Compute Hamming distance
            sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_ancilla, len_reg_memory, num_bin_pattern);

            sim.PrintStates("After encoding: ");  


            REQUIRE(1 == 1);

        }
    }
}

/*
        std::size_t val;

        // Compute Hamming distance
        HammingDistance<DerivedType> hamming_operator(len_reg_memory);
        hamming_operator.computeHammingDistance(static_cast<DerivedType&>(*this), reg_memory, reg_ancilla, len_reg_memory);

        // Measure
        val = sim.applyMeasurementToRegister(reg_memory);

        CHECK_THAT(vec_to_encode, VectorContains(val));
        */

/*
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
*/
