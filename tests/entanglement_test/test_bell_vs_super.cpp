#include "utils/util.hpp"
#include "nqubit_decompose.hpp"

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <bitset>

using namespace QNLP;

void test_sub_2bits(){
    unsigned int r1_size = 2;
    unsigned int num_qubits = r1_size;
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);

    auto output = [&](QubitRegister<ComplexDP> psi1, unsigned int num_qubits) { 
        for(int bits = 0; bits < pow(2,num_qubits) ; bits++){
            std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
        }
        for(int i = 0; i < 2 ; i++){
            std::cout << psi1.GetProbability(i) << "\t";    
        }
        std::cout << std::endl;
        return; 
    };

    std::cout << "###########################################" << std::endl;
    for(int i = 0; i < 2; i++){
        psi1.ApplyHadamard(i);
    }
    output(psi1, num_qubits);

    std::cout << "*******************************************" << std::endl;
    std::cout << "|Psi1>=(|1><1\\otimes I)(|Psi>)" << std::endl;
    psi1.CollapseQubit(0, true);
    psi1.Normalize();
    output(psi1,num_qubits);

    std::cout << "*******************************************" << std::endl;
    std::cout << "(I\\otimes|1><1)(|Psi1>)" << std::endl;
    psi1.CollapseQubit(1, true);
    psi1.Normalize();
    output(psi1,num_qubits);

    std::cout << "###########################################" << std::endl;
    
    psi1.Initialize("base",0);
    psi1.ApplyHadamard(0);
    psi1.ApplyCPauliX(0,1);
    output(psi1,num_qubits);

    std::cout << "*******************************************" << std::endl;
    std::cout << "|Psi1>=(|1><1\\otimes I)(|Psi>)" << std::endl;
    psi1.CollapseQubit(0, true);
    psi1.Normalize();
    output(psi1,num_qubits);

    std::cout << "*******************************************" << std::endl;
    std::cout << "(I\\otimes|1><1)(|Psi1>)" << std::endl;
    psi1.CollapseQubit(1, true);
    psi1.Normalize();
    output(psi1,num_qubits);

    std::cout << "###########################################" << std::endl;
}
int main(int argc, char **argv){
    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();
    if (rank == 0) {
        printf("\n --- BELL vs SUPERPOS --- \n");
    }
    test_sub_2bits();
    return 0;
}
