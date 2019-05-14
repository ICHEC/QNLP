#define CATCH_CONFIG_RUNNER

#include "ncu.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;

template class NCU< openqu::TinyMatrix< complex<double>, 2, 2, 32 >, IntelSimulator>;

TEST_CASE("Test n-controlled unitary","[ncu]"){
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
                    NCU<decltype(sim.getGateX()), decltype(sim)> nqd(sim.getGateX(), num_qubits-1);
                    nqd.applyNQubitControl(sim, c_start, c_end, target, sim.getGateX(), 0, true); //isX = true

                    if(pattern_idx != pattern_total-1){
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(0.0).margin(1e-12));
                    }
                    else{
                        REQUIRE(sim.getQubitRegister().GetProbability( target )  == Approx(1.0).margin(1e-12));
                    }
                    //std::cout << "Test " << pattern_idx << "\t\tPattern=" << std::bitset<8>(pattern_idx);
                    //std::cout << "\t\tP(0)=" << 1. - sim.getQubitRegister().GetProbability( target );
                    //std::cout << "\t\tP(1)=" << sim.getQubitRegister().GetProbability( target );
                    //std::cout<< std::endl;
                }
            }

        }

    }
}

int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}

/**
 * @brief Tests the n-qubit controlled gate decomposition for all test patterns given the supplied number of qubits
 *
 */
/*
   void test_decompose(std::size_t num_qubits, openqu::TinyMatrix<Type, 2, 2, 32> U, bool isX){
//For a given register of size num_qubits, the control lines are the first to last-1 
//indexed qubits, and the target is the last.
unsigned int c_start = 0, c_end = num_qubits-2;
unsigned int target = num_qubits-1;

QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);

//Measure the probability of the target state being set by applying all bit-patterns of control-qubits
//for the given number of qubits.
double p = -1.;
for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
psi1.Initialize("base",0);

for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
if(window_select == 1){
psi1.ApplyPauliX(ctrl_qubit);
}
}
NQubitDecompose<ComplexDP> nqd(U, num_qubits-1);
nqd.applyNQubitControl(psi1, c_start, c_end, target, U, 0, isX);
std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
std::cout << "\t\tP(0)=" << 1. - psi1.GetProbability( target );
std::cout << "\t\tP(1)=" << psi1.GetProbability( target );
std::cout<< std::endl;
}
}

int main(int argc, char **argv){

openqu::mpi::Environment env(argc, argv);
if (env.is_usefull_rank() == false) return 0;

int rank = env.rank();

{
if (rank == 0) {
printf("\n --- n-Qubit Controlled U Test --- \n");
std::cout << "The n-qubit register (gates 0 to n-1) is initialized in state |0..0>. \n"
<< "The controlled U test uses U=X, and as such implements an (n-1)-qubit controlled Not decomposition.  n"
<< "The resulting logic table should operate as follows (for n=3, 2 control, 1 target) \n"
<< "C C -> T\n"
<< "0 0 -> 0\n"
<< "0 1 -> 0\n"
<< "1 0 -> 0\n"
<< "1 1 -> 1\n";
}
openqu::TinyMatrix<Type, 2, 2, 32> X;
X(0,0) = {0.,  0.};
X(0,1) = {1., 0.};
X(1,0) = {1., 0.};
X(1,1) = {0.,  0.};
//Hard-coded number of tests to perform. Starts with a 3-qubit register and increments the number for each subsequent test
unsigned int num_tests=7;
//        std::cin >> num_tests;
if (rank == 0){
for (unsigned int i=0; i < num_tests; i++){
std::cout << "################################################" << std::endl;
std::cout << "Testing "<< i+2 <<" control lines" << std::endl;
test_decompose(i+3, X, true);
std::cout << "################################################" << std::endl;
}
}
}
}

*/
