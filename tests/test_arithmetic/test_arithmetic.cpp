#include "utils/util.hpp"
#include "ncu.hpp"

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

void test_sub_2bits(){
    unsigned int r1_size = 3, r2_size=3;
    unsigned int num_qubits = r1_size + r2_size;
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    unsigned int    r1_min = 0, 
                    r1_max = 2, 
                    r2_min = 3, 
                    r2_max = 5; 
    std::cout << "Register index ranges: r1={" << r1_min <<"," << r1_max << "} r2={" << r2_min << "," << r2_max << "}" << std::endl;

    //for(int i = r2_min; i < r2_max; i++){
    //    psi1.ApplyHadamard(i);
    //}
    psi1.ApplyPauliX(r2_min + 1);
//    psi1.ApplyPauliX(r2_min);

    psi1.ApplyPauliX(r1_min + 1);
//    psi1.ApplyPauliX(r1_min);
    for(int i = r1_min; i <= r2_max ; i++){
        std::cout << psi1.GetProbability(i) << "\t";    
    }
    std::cout << std::endl;
    for(int bits = 0; bits < pow(2,num_qubits) ; bits++){
        std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
    }
    std::cout << "###########################################" << std::endl;
    Util::sub_reg(psi1, r1_min, r1_max, r2_min, r2_max);
    psi1.ApplyPauliZ(r2_min);
    for(int i = r1_min; i <= r2_max ; i++){
        std::cout << psi1.GetProbability(i) << "\t";    
    }
    for(int bits = 0; bits < pow(2,num_qubits) ; bits++){
        std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
    }
    std::cout << "###########################################" << std::endl;
    
}

/**
 * @brief Test conditional inverse for negative amplitudes. Candidate for Hamming distance threshold + Grover diffusion method
 *
 */
void test_sub_amplitude_inversion(){
    unsigned int r1_size = 2, r2_size=2, r3_size=1;
    unsigned int num_qubits = r1_size + r2_size + r3_size;
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    unsigned int    r1_min = 0, 
                    r1_max = r1_min + r1_size - 1, 
                    r2_min = r1_max + 1, 
                    r2_max = r2_min + r2_size - 1, 
                    r3_min = r2_max + 1, 
                    r3_max = r3_min + r3_size - 1;
    std::cout << "Register index ranges: r1={" << r1_min <<"," << r1_max << "} r2={" << r2_min << "," << r2_max << "} r3={" << r3_min << "," << r3_max << "}" << std::endl;

    for(int i = r2_min; i <= r2_max; i++){
        psi1.ApplyHadamard(i);
    }

    //Lambda for RY
    auto RY = [](double theta) { 
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> RY;
        RY(0,0) = {cos(theta/2),  0.};
        RY(0,1) = {-sin(theta/2), 0.};
        RY(1,0) = {sin(theta/2), 0.};
        RY(1,1) = {cos(theta/2),  0.};
        return RY; 
    };

    //Create state : |00>(|00>|0> + |01>Ry(pi/6)(|0>) + |10>Ry(pi/3)(|0>) + |11>Ry(pi/2)(|0>))
    NCU<ComplexDP> nCtrlRY(RY(M_PI/8), r1_size);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);

    psi1.ApplyPauliX(r1_min);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);
    psi1.ApplyPauliX(r1_min);

    psi1.ApplyPauliX(r1_min+1);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);
    nCtrlRY.applyNQubitControl(psi1, r2_min, r2_max, r3_min, RY(M_PI/8), 0, false);
    psi1.ApplyPauliX(r1_min+1);

    //Encode threshold in r1 as |10>
    psi1.ApplyPauliX(r1_min+1);

    //State is now : |10>(|00>|0> + |01>Ry(pi/6)(|0>) + |10>Ry(pi/3)(|0>) + |11>Ry(pi/2)(|0>))
    //Perform subtraction between r1 and r2 so that |r1>|r2> -> |r1>|r1-r2>
    


   
    for(int bits = 0; bits < pow(2,num_qubits) ; bits++){
        std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
        if(psi1[bits].real() <0){
            std::cout << "HERE! " << bits << std::endl;
        }
    }
    
    Util::sub_reg(psi1, r1_min, r1_max, r2_min, r2_max);
    psi1.ApplyPauliZ(r3_min);

    std::cout << "2 - [0..3]" << std::endl;
    for (unsigned int j = r1_min; j <= r3_max; j++){
        std::cout << psi1.GetProbability( j ) << "\t";
    }
    std::cout << std::endl;

    for(int bits = 0; bits < pow(2,num_qubits) ; bits++){
        std::cout << "Bit[" << bits << "]=" << psi1[bits] << std::endl;
        if(psi1[bits].real() <0){
            std::cout << "HERE! " << bits << std::endl;
        }
    }
    Util::ApplyDiffusionOp(psi1, r2_min, r2_max);
    std::cout << "##################################################" << std::endl;
    std::cout << "P=" << std::endl;
    for (unsigned int j = r2_min; j <= r2_max; j++){
        std::cout << psi1.GetProbability( j ) << "\t";
    }
    std::cout << std::endl;
    std::cout << "##################################################" << std::endl;
}
int main(int argc, char **argv){
    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();
    if (rank == 0) {
        printf("\n --- Arithmetic ops --- \n");
    }
    unsigned int num_qubits = 8;

    //test_sub(num_qubits);
    //test_sum(num_qubits);
    //test_sub_amplitude_inversion();
    test_sub_2bits();
    return 0;
}
