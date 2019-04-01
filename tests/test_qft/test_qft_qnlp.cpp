#include "qft.hpp"

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <bitset>

using namespace QNLP;

void test_qft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
     
    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);
            
        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }
        Ops::applyQFT(psi1, 0, num_qubits);
        std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

void test_iqft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    
    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);
            
        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }

        Ops::applyIQFT(psi1, 0, num_qubits);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

void test_qft_iqft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    
    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);
            
        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }

        Ops::applyQFT(psi1, 0, num_qubits);
        Ops::applyIQFT(psi1, 0, num_qubits);
        std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    {
        if (rank == 0) {
            printf("\n --- n-Qubit QFT --- \n");
        }
        unsigned int max_qubits=7, min_qubits=2;;
//        std::cin >> num_tests;
        if (rank == 0){
            for (unsigned int i = min_qubits; i < max_qubits; i++){
                std::cout << "################################################" << std::endl;
                std::cout << "Testing "<< i <<" qubits QFT" <<  std::endl;
                test_qft(i);
                std::cout << "Testing "<< i <<" qubits IQFT" <<  std::endl;
                test_iqft(i);
                std::cout << "Testing "<< i <<" qubits QFT-IQFT" <<  std::endl;
                test_qft_iqft(i);
                std::cout << "################################################" << std::endl;

            }
        }
    }

}
