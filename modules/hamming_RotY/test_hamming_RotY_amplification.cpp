//#define CATCH_CONFIG_RUNNER

#include "hamming_RotY_amplification.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;
using namespace Catch::Matchers;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class HammingDistanceRotY<IntelSimulator>;

//using namespace Catch::Detail;

TEST_CASE("Test Hamming distance with Roatation about y axis routine","[hammingroty]"){
    const std::size_t max_qubits = 3;
    double mach_eps = 7./3. - 4./3. -1.;

    std::size_t num_qubits;
    std::size_t len_reg_ancilla;
    std::size_t num_bin_pattern;
    std::size_t val;

    std::size_t test_pattern = 2;

    //Catch::literals::Approx approx_zero = Catch::literals::Approx(0.).margin(1e-12);

    for(std::size_t len_reg_memory = 2; len_reg_memory < max_qubits; len_reg_memory++){
        DYNAMIC_SECTION("Testing " << len_reg_memory << " memory qubits"){

            // Experiment set-up
            num_qubits = 2*len_reg_memory + 2;
            len_reg_ancilla = len_reg_memory + 2;
            num_bin_pattern = pow(2,len_reg_memory);

            IntelSimulator sim(num_qubits);
            auto &r = sim.getQubitRegister();

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
            for(std::size_t i = 0; i < num_bin_pattern; i++){
                vec_to_encode[i] = i;
            }

            // Encode
            sim.encodeBinToSuperpos_unique(reg_memory, reg_ancilla, vec_to_encode, len_reg_memory);

            // Compute Hamming distance
            sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_ancilla, len_reg_memory, num_bin_pattern);


            sim.PrintStates("After Hamming: ");  

            for(int i = 0; i < num_bin_pattern; i++){
                cout << i << "\t" << r[i] << endl;
            }
cout << "------------" << endl;
            for(int i = num_bin_pattern; i < pow(2,2*len_reg_memory); i++){
                cout << i << "\t" << r[i] << endl;
                REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );
            }
 cout << "------------" << endl;
            for(int i = pow(2,2*len_reg_memory); i < pow(2,2*len_reg_memory) + num_bin_pattern; i++){
                cout << i << "\t" << r[i] << endl;
            }
  cout << "------------" << endl;
            for(int i = pow(2,2*len_reg_memory) + num_bin_pattern; i < pow(2,num_qubits); i++){
                cout << i << "\t" << r[i] << endl;
                REQUIRE(r[i].real() + 10*mach_eps == Approx(0.).margin(1e-12));
                REQUIRE(r[i].imag() + 10*mach_eps == Approx(0.).margin(1e-12) );

            }
        

            sim.collapseToBasisZ(reg_ancilla[len_reg_ancilla-2], 1);
            val = sim.applyMeasurementToRegister(reg_memory);

//            sim.PrintStates("After Measurement: ");  

            CHECK_THAT(vec_to_encode, VectorContains(val));

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
