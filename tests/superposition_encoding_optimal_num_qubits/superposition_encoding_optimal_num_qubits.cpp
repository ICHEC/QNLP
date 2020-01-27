/**
 * @file superposition_encoding.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Encodes binary strings based on Trugenberger's Quantum Pattern Recognition (arXiv:quant-ph/0210176v2)
 * @version 0.1
 * @date 2019-04-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "QubitCircuit.hpp"
//#include <bitset>
#include "ncu.hpp"
using namespace QNLP;

double EPSILON = 1e-6;

/**
 * @brief Checks if the bit'th bit of the integer byte is set
 * 
 * @param byte - integer representing a binary string
 * @param bit - the index of the bit to be checked (beginning
 * with the least significant bit)
 */
#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))


/**
 * @brief Print binary representation of input value of length len
 * 
 * @param val - positive integer input
 * @param len - length of the binary string to be printed beginning at LSB.
 */
void print_bits(unsigned int val, int len){
    for (int i = len-1; i > -1; i--){
        std::cout << ((val >> i) & 1UL);
    }
}


/**
 * @brief Generates the U^i_j operator required for a given bit
 * that is to be encoded. 
 * U^i_j = (cos(pi/2 p^i_j)*Identity + i*sin(pi/2 p^i_j)*PauliY)
 * 
 * @param bit_pattern - Bit value being encoded
 * @param op_U - A 2x2 matrix which stores the resulting operator
 */
template<class Type>
void generate_operator_U(bool bit, openqu::TinyMatrix<Type,2,2,32>& op_U){
    op_U(0,0) = {1.0*(bit),0.0};
    op_U(0,1) = {1.0*(!bit),0.0};
    op_U(1,0) = {-1.0*(!bit),0.0};
    op_U(1,1) = {1.0*(bit),0.0};
}

/**
 * @brief Generates the inverse U^i_j operator required for a given bit
 * that is to be encoded. 
 * (U^i_j)^-1 = (cos(pi/2 p^i_j)*Identity + i*sin(pi/2 p^i_j)*PauliY)^-1
 * 
 * @param bit_pattern - Bit value being encoded
 * @param op_U_inv - A 2x2 matrix which stores the resulting operator
 */
template<class Type>
void generate_operator_U_inv(bool bit, openqu::TinyMatrix<Type,2,2,32>& op_U_inv){
    op_U_inv(0,0) = {1.0*(bit),0.0};
    op_U_inv(0,1) = {-1.0*(!bit),0.0};
    op_U_inv(1,0) = {1.0*(!bit),0.0};
    op_U_inv(1,1) = {1.0*(bit),0.0};
}





/**
 * @brief Generates the P^i operator required for a given binary string 
 * that is to be encoded. 
 * P^i = prod_j(cos(pi/2 p^i_j)*Identity + i*sin(pi/2 p^i_j)*PauliY)
 * 
 * @param circ - Quantum circuit
 * @param bit_pattern - Binary pattern represented as unsigned integer
 * @param len_bit_pattern - length of binary pattern
 * @param op_P - A 2x2 matrix which stores the resulting operator
 */
template<class Type>
void generate_operator_P(QubitCircuit<Type>&circ, unsigned bit_pattern, unsigned len_bit_pattern, openqu::TinyMatrix<Type,2,2,32>& op_P){

    assert(len_bit_pattern > 0);

    bool bit, accum;
    int bool_parity = 1;

    bit = IS_SET(bit_pattern,0);
    accum = bit;
    for(int j = 1; j < len_bit_pattern; j++){
        bit = IS_SET(bit_pattern,j);
        parity *= -1*(bit & accum) + 1*(!(bit & accum));
        accum ^= bit;
    }

    op_P(0,0) = {1.0*parity*(!accum),0.0};
    op_P(0,1) = {1.0*parity*(accum),0.0};
    op_P(1,0) = {-1.0*parity*(accum),0.0};
    op_P(1,1) = {1.0*parity*(!accum),0.0};
}

// Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
//              - Let there exist m vectors of binary number, each of length n.
/**
 * @brief 
 * 
 * @tparam Type 
 * @param pattern - binary strings in integer format to be encoded
 * @param circ - Quantum circuit
 * @param qRegCirc - Instance of class containing indices for each register int eh circuit 
 * @param S - Matrices required for the encoding algorithm (one for each input)
 * @param op_nCDecomp - Method defining the operator for nCU
 * s
 * M
 * @param X - The unitary matrix U of the nCU operation (in this case Pauli-X matrix)
 */
template < class Type >
void encode_binarystrings(QubitCircuit<Type>& circ, vector<unsigned>& reg_memory, vector<unsigned>& reg_auxiliary, vector<unsigned int>& pattern,  vector<openqu::TinyMatrix<Type, 2, 2, 32>>& S, int m, int n){
//NCU<ComplexDP>& op_nCDecomp, openqu::TinyMatrix<Type, 2, 2, 32>& X){

    // Encode

    // Prepare state in |0...0>|10> of lengths n,2
    circ.ApplyPauliX(reg_auxiliary[1]);

    openqu::TinyMatrix<Type,2,2,32> op_U; 

    for(int i = 0; i < m; i++){

        for(int j = 0; j < n; j++){
            // Generates U^i operator in op_U for this bit
            generate_operator_U(IS_SET(pattern[i],j), op_U);
            // Apply U^i to j with control u2
            circ.ApplyControlled1QubitGate(reg_auxiliary[1], reg_memory[j], op_U);
        }

        circ.ApplyCPauliX(reg_auxiliary[1],reg_auxiliary[0]);


        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        circ.ApplyControlled1QubitGate(reg_auxiliary[0], reg_auxiliary[1], S[i]);

        circ.ApplyPauliX(reg_auxiliary[0]);


        // Reset the memory register of the new term to the state |0...0>
        for(int j = n-1; j > -1; j--){
            // Generates (U^i)^-1 operator in op_U for this bit
            generate_operator_U_inv(IS_SET(pattern[i],j), op_U);
            // Apply (U^i)^-1 to j with control u2
            circ.ApplyControlled1QubitGate(reg_auxiliary[1], reg_memory[j], op_U);
        }
    }
}




int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned m, n, total_qubits, num_exps;
    unsigned num_classes, numQubits_class;

    m = 4;
    n = 6;
    num_exps = 500;

    total_qubits = n + 2;

    // To store initial patterns 
    vector<unsigned int> pattern(m);
    vector<unsigned int> input_pattern(1);

    pattern[0] = 63;
    pattern[1] = 32;
    pattern[2] = 56;
    pattern[3] = 7;

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits,"base",0);

    // Define Memory and auxiliary registers by setting their indices
    // into a corresponding vector
    vector<unsigned> reg_memory(n);
    vector<unsigned> reg_auxiliary(2);
    for(int j = 0; j < n; j++){
        reg_memory[j] = j;
    }
    for(int j = 0; j < 2; j++){
        reg_auxiliary[j] = j + n;
    }


    // Preparation for binary encoding:
    //
    // Prepare three matrices S^1,S^2,...,S^3 that are required for the implemented
    // algorithm to encode these binary strings into a superposition state.
    //
    // Note the matrix indexing of the S vector of S^p matrices will be backwards:
    //      S[0] -> S^p
    //      S[1] -> S_{p-1}, and so on.
    //
    vector<openqu::TinyMatrix<ComplexDP, 2, 2, 32>> S(m);
    {
        int p = m;
        double diag, off_diag;

        for(int i = 0; i < m; i++){
            off_diag = 1.0/sqrt((double)(p));
            diag = off_diag * sqrt((double)(p-1));

            S[i](0,0) = {diag, 0.0};
            S[i](0,1) = {off_diag, 0.0};
            S[i](1,0) = {-off_diag, 0.0};
            S[i](1,1) = {diag, 0.0};

            p--;
        }
    }






    vector<double> count(m);
    vector<double> prob_dist(m);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0,1.0);
    double dart, partition;
    double average = 0;
    unsigned partition_id;

    int exp = 0;
    while(exp < num_exps){
        // Re-init
        average = 0.0;
        for(vector<double>::iterator it = prob_dist.begin(); it != prob_dist.end(); it++){
            *it = 1.0;
        }
        circ.Initialize("base",0);


        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(circ, reg_memory, reg_auxiliary, pattern, S, m, n);

        // Collapse qubits to state randomly selectd in class register
        for(int j = 0; j < n; j++){
            dart = dist(mt);
            circ.CollapseQubit(reg_memory[j],(dart < circ.GetProbability(reg_memory[j])));
            circ.Normalize();
        }

        // Store current state of training register in it's integer format
        int val = 0;
        for(int j = n-1; j > -1; j--){
            val |= ((unsigned int)circ.GetProbability(reg_memory[j]) << j);
        }

        // Increase the count of the training pattern measured
        bool flag = false;
        for(int i = 0; i < m; i++){
            if(pattern[i] == val){
                count[i]++;
                flag = true;
            }
        }
        if(!flag){
            print_bits(val,n);
            cout << endl;

        }

        exp++;
    }

    if(rank == 0){
        double dist;
        double prob, prob_sum, prob_cos_term;
        prob_sum = 0.0;

        cout << endl << "Expected output probability for each sample = " << 1.0/(double) m << endl << endl;
        cout << " \tPattern\t\t\tfreq\tprob" << endl;
        for(int i = 0; i < m; i++){

            prob = count[i]/(double) num_exps;
            prob_sum += prob;

            cout << i << " \t|";
            print_bits(pattern[i], n);
            cout << ">\t\t" << count[i] << "\t" << count[i]/(double)num_exps * 100.0 << "%" << endl;
        }
        cout << "Sum of probs =\t" << prob_sum << endl << endl;

        cout << "Number of patterns: " << m << endl;
        cout << "Pattern length: " << n << endl;
        cout << "Number of required qubits: " << total_qubits << endl;
    }

    return 0;

}



#ifdef NEVER
/**
 * @file encoding.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Encodes binary strings based on Trugenberger's Quantum Pattern Recognition (arXiv:quant-ph/0210176v2)
 * @version 0.1
 * @date 2019-04-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "util/tinymatrix.hpp"

#include "QubitCircuit.hpp"
#include <bitset>

#include "ncu.hpp"
using namespace QNLP;

double EPSILON = 1e-6;
    openqu::TinyMatrix<ComplexDP, 2, 2, 32> X;
    X(0,0) = {0.,  0.};
    X(0,1) = {1., 0.};
    X(1,0) = {1., 0.};
    X(1,1) = {0.,  0.};
    NCU<ComplexDP> op_nCDecomp(X, n);

    double dart, partition;
    unsigned partition_id;

    int exp = 0;
    while(exp < num_exps){
        // Re-init
        average = 0.0;
        for(vector<double>::iterator it = prob_dist.begin(); it != prob_dist.end(); it++){
            *it = 1.0;
        }
        circ.Initialize("base",0);

        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(pattern, circ, qRegCirc, S, op_nCDecomp,X);

        // Collapse qubits to state randomly selectd in class register
        for(int j = 0; j < n; j++){
            dart = dist(mt);
            circ.CollapseQubit(qRegCirc.get_mReg(j),(dart < circ.GetProbability(qRegCirc.get_mReg(j))));
            circ.Normalize();
        }

        // Store current state of training register in it's integer format
        int val = 0;
        for(int j = n-1; j > -1; j--){
            val |= ((unsigned int)circ.GetProbability(qRegCirc.get_mReg(j)) << j);
        }

        // Increase the count of the training pattern measured
        bool flag = false;
        for(int i = 0; i < m; i++){
            if(pattern[i] == val){
                count[i]++;
                flag = true;
            }
        }
        if(!flag){
            print_bits(val,n);
            cout << endl;

        }

        exp++;
    }

    if(rank == 0){
        double dist;
        double prob, prob_sum, prob_cos_term;
        prob_sum = 0.0;

        cout << endl << "Expected output probability for each sample = " << 1.0/(double) m << endl << endl;
        cout << " \tPattern\t\t\tfreq\tprob" << endl;
        for(int i = 0; i < m; i++){

            prob = count[i]/(double) num_exps;
            prob_sum += prob;

            cout << i << " \t|";
            print_bits(pattern[i], n);
            cout << ">\t\t" << count[i] << "\t" << count[i]/(double)num_exps * 100.0 << "%" << endl;
        }
        cout << "Sum of probs =\t" << prob_sum << endl << endl;

        cout << "Number of patterns: " << m << endl;
        cout << "Pattern length: " << n << endl;
        cout << "Number of required qubits: " << total_qubits << endl;
    }

    return 0;

}

#endif
