#include "utils/util.hpp"

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <bitset>

using namespace QNLP;

/**
 * @brief Calculates the subtractin of bits in quantum register. Assumes the given register is partitioned in 2: |reg> = |a>|b>
 * where the routine calculates: |a>|b> -> |a>|a-b>. Underflow possible, so values within range/2 should be used to avoid flipping first 
 * qubit in |b>
 */
void test_sub(unsigned int num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    unsigned int r1_min = 0, r1_max=(num_qubits/2)-1, r2_min = r1_max+1, r2_max = num_qubits-1;
    for(int pattern = 0; pattern < 1<<(num_qubits/2); pattern ++){
        unsigned int bitmask = 0x1;
        unsigned int a = 0;
        psi1.Initialize("base",0);
        for (unsigned int i=0; i <= r1_max -1; i++){
            psi1.ApplyPauliX(i);
            a += (unsigned int) pow(2,i);
        }
        for (unsigned int j = r2_min; j <= r2_max; j++){
            //std::cout  << "PATTERN:=" << pattern << "    BITMASK" << bitmask << "    AND=" << (pattern & bitmask) << "\n";
            if ( (pattern & bitmask) > 0 ){
                psi1.ApplyPauliX(j);
            }
            bitmask <<=1;
        }
        Util::sub_reg(psi1, r1_min, r1_max, r2_min, r2_max);
        std::string result="";

        for (unsigned int j = r2_min; j <= r2_max; j++){
            result += std::to_string(psi1.GetProbability( j )).at(0);
        }
        std::reverse(result.begin(), result.end());
        std::cout << "Sub = " << std::bitset<4>(a) << "-" << std::bitset<4>(pattern) << "=> Expected:=" << std::bitset<4>(a-pattern) << " : Actual:=" << result << std::endl;
        //psi1.ApplyPauliZ(r2_min);
        //for(int bits = 0; bits < pow(2,num_qubits) ; bits++)
        //    std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
    }
}

/**
 * @brief Calculates the summation of two n/2 bit integers in quantum register. Assumes the given register is partitioned in 2: |reg> = |a>|b>
 * where the routine calculates: |a>|b> -> |a>|a+b>. Overflow possible, so values within range/2 should be used to avoid flipping first 
 * qubit in |b>
 */
void test_sum(unsigned int num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    unsigned int r1_min = 0, r1_max=(num_qubits/2)-1, r2_min = r1_max+1, r2_max = num_qubits-1;
    for(int pattern = 0; pattern < 1<<(num_qubits/2); pattern++){
        unsigned int bitmask = 0x1;
        unsigned int a = 1;
        psi1.Initialize("base",0);
        psi1.ApplyPauliX(0); //Adding numbers to 1
        for (unsigned int j = r2_min; j <= r2_max; j++){
            std::cout  << "PATTERN:=" << pattern << "    BITMASK" << bitmask << "    AND=" << (pattern & bitmask) << "    J=" << j <<"\n";
            if ( (pattern & bitmask) > 0 ){
                psi1.ApplyPauliX(j);
            }
            bitmask <<= 1;
        }
        Util::sum_reg(psi1, r1_min, r1_max, r2_min, r2_max);
        std::string result="";

        for (unsigned int j = r2_min; j <= r2_max; j++){
            std::cout << "P(j=" << j << ")=" << std::to_string(psi1.GetProbability( j )).at(0) << std::endl;
            result += std::to_string(psi1.GetProbability( j )).at(0);
        }
        std::reverse(result.begin(), result.end());
        std::cout << "Sum = " << std::bitset<4>(a) << "+" << std::bitset<4>(pattern) << "=> Expected:=" << std::bitset<4>(a+pattern) << " : Actual:=" << result << std::endl;
        //psi1.ApplyPauliZ(r2_min);
        //for(int bits = 0; bits < pow(2,num_qubits) ; bits++)
        //    std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
    }
}

int main(int argc, char **argv){
    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();
    if (rank == 0) {
        printf("\n --- Arithmetic ops --- \n");
    }
    unsigned int num_qubits = 8;

    test_sub(num_qubits);
    test_sum(num_qubits);
    return 0;
}
