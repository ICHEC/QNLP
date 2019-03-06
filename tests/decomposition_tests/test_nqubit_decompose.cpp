#include "nqubit_decompose.hpp"

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include<bitset>

using namespace QNLP;

typedef ComplexDP Type;

void test_decompose(std::size_t num_qubits, openqu::TinyMatrix<Type, 2, 2, 32> U, bool isX){
    unsigned int c_start = 0, c_end = num_qubits-2;
    unsigned int target = num_qubits-1;

    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    unsigned int shifts;
    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        shifts = 0;
        psi1.Initialize("base",0);
            
        for( unsigned int q = 1; q <= cc; q = q << 1){
            if(cc & q > 0){
                psi1.ApplyPauliX(shifts);
            }
            shifts++;
        } 
        NQubitDecompose<ComplexDP> nqd(U);
        nqd.applyNQubitControl(psi1, c_start, c_end, target, U, 0, isX);
        std::cout << "Test " << cc << " Pattern=" << std::bitset<8>(cc);
        std::cout << " P(1)=" << psi1.GetProbability( target ) << std::endl;
    }
}

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int myid = env.rank();

    {
        if (myid == 0) {
            printf("\n --- n-Qubit Controlled U Test --- \n");
            std::cout << "The 3-qubit register (gates 0 to 2) is initialized in state |000>. \n"
                << "The controlled U test uses U=X, and as such implements a Toffoli decomposition. \n"
                << "The resulting logic table should operate as follows \n"
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
        unsigned int num_tests=1;
        int i = 1;
//        std::cin >> num_tests;
        if (myid == 0){
//            for (unsigned int i=0; i < num_tests; i++){
                std::cout << "################################################" << std::endl;
                std::cout << "Testing "<< i+2 <<" control lines" << std::endl;
                test_decompose(i+3, X, true);
                std::cout << "################################################" << std::endl;

//            }
        }
    }

}
