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

template class HammingDistance<IntelSimulator>;


TEST_CASE("Test Hamming distance routine","[Hamming]"){
    SECTION("Testing distance calc"){
/*
        std::size_t num_qubits = 10;

        std::size_t test_pattern = 2;

        std::size_t len_reg_memory = (num_qubits - 2) / 2;
        std::size_t len_reg_ancilla = len_reg_memory + 2;
        std::size_t num_bin_pattern = pow(2,len_reg_memory) - 2;

        IntelSimulator sim(num_qubits);
        REQUIRE(sim.getNumQubits() == num_qubits);

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

        std::size_t val;
        // Encode
        sim.encodeBinToSuperpos_unique(reg_memory, reg_ancilla, vec_to_encode, len_reg_memory);

        // Compute Hamming distance
        HammingDistance<DerivedType> hamming_operator(len_reg_memory);
        hamming_operator.computeHammingDistance(static_cast<DerivedType&>(*this), reg_memory, reg_ancilla, len_reg_memory);

        // Measure
        val = sim.applyMeasurementToRegister(reg_memory);

        CHECK_THAT(vec_to_encode, VectorContains(val));
        */
    }
}

/*
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
*/
