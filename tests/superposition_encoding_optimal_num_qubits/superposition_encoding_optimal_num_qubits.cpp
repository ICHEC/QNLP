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
 * @param X - The unitary matrix U of the nCU operation (in this case Pauli-X matrix)
 */
template < class Type >
void encode_binarystrings(vector<unsigned int>& pattern, QubitCircuit<Type>& circ, myRegisters& qRegCirc, vector<openqu::TinyMatrix<Type, 2, 2, 32>>& S, NCU<ComplexDP>& op_nCDecomp, openqu::TinyMatrix<Type, 2, 2, 32>& X){

    // Encode
    int m,n;
    m = qRegCirc.get_numTrainStrings();
    n = qRegCirc.get_lenTrainStrings();

    // Prepare state in |0...>|0...0>|10> of lengths n,n,2
    circ.ApplyPauliX(qRegCirc.get_cReg(1));


    for(int i = 0; i < m; i++){
        // Psi0
        // Encode inputted binary pattern to pReg
        for(int j = 0; j < n; j++){
            if(IS_SET(pattern[i],j)){
                circ.ApplyPauliX(qRegCirc.get_pReg(j));
            }
        }

        // Psi1
        // Encode inputted binary pattern
        for(int j = 0; j < n; j++){
            circ.ApplyToffoli(qRegCirc.get_pReg(j), qRegCirc.get_cReg(1), qRegCirc.get_mReg(j));
        }

        // Psi2
        for(int j = 0; j < n; j++){
            circ.ApplyCPauliX(qRegCirc.get_pReg(j), qRegCirc.get_mReg(j));
            circ.ApplyPauliX(qRegCirc.get_mReg(j));
        }

        // Psi3
        op_nCDecomp.applyNQubitControl(circ, qRegCirc.get_mReg(0), qRegCirc.get_mReg(n-1), qRegCirc.get_cReg(0), X, 0, true);

        // Psi4
        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        circ.ApplyControlled1QubitGate(qRegCirc.get_cReg(0), qRegCirc.get_cReg(1), S[i]);

        // Psi5
        op_nCDecomp.applyNQubitControl(circ, qRegCirc.get_mReg(0), qRegCirc.get_mReg(n-1), qRegCirc.get_cReg(0), X, 0, true);

        // Psi6 
        for(int j = n-1; j > -1; j--){
            circ.ApplyPauliX(qRegCirc.get_mReg(j));
            circ.ApplyCPauliX(qRegCirc.get_pReg(j), qRegCirc.get_mReg(j));
        }

        // Psi7
        for(int j = n-1; j > -1; j--){
            circ.ApplyToffoli(qRegCirc.get_pReg(j), qRegCirc.get_cReg(1), qRegCirc.get_mReg(j));
        }

        // Reset the p register of the new term to the state |0...0>
        for(int j = 0; j < n; j++){
            // Check current pattern against next pattern
            bool p1, p2;
            p1 = IS_SET(pattern[i],j);
            if(p1){
                circ.ApplyPauliX(qRegCirc.get_pReg(j));
            }

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

    total_qubits = 2*n + 2;

    // To store initial patterns 
    vector<unsigned int> pattern(m);
    vector<unsigned int> input_pattern(1);

    pattern[0] = 63;
    pattern[1] = 32;
    pattern[2] = 56;
    pattern[3] = 7;

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits,"base",0);

    // Define Memory and ancilla registers by setting their indices
    // into a corresponding vector
    vector<unsigned> reg_memory(n);
    vector<unsigned> reg_ancilla(n+2);
    for(int j = 0; j < n; j++){
        reg_memory[j] = j;
    }
    for(int j = 0; j < n+2; j++){
        reg_ancilla[j] = j + n;
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
        //encode_binarystrings<ComplexDP>(pattern, circ, qRegCirc, S, op_nCDecomp,X);
        circ.EncodeBinInToSuperposition(reg_memory, reg_ancilla, pattern, n);

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


/**
 * @brief Application which 
 *      - encodes a set of binary strings into a superposition state
 */


/**
 * @brief Class to Update and store the global circuit indices for each register
 */
class myRegisters{
    private:
        unsigned m, n;
        // Use vectors to store indices of appropriate registers in circuit 
        vector<unsigned> pReg;
        vector<unsigned> mReg;
        vector<unsigned> cReg;
        
    public:
        myRegisters(int m_, int n_) : m(m_), n(n_), pReg(n), mReg(n), cReg(2){
            
            for(int i = 0; i < n; i++){
                pReg[i] = i;
            }
            for(int i = 0; i < n; i++){
                mReg[i] = i + n;
            }
            cReg[0] = 2*n;
            cReg[1] = 2*n+1;
        }

        unsigned get_numTrainStrings(){
            return m;
        }
        unsigned get_lenTrainStrings(){
            return n;
        }

        unsigned get_pReg(unsigned i){
            assert(i < n);
            return pReg[i];
        }
        unsigned get_mReg(unsigned i){
            assert(i < n);
            return mReg[i];
        }
        unsigned get_cReg(unsigned i){
            assert(i < 2);
            return cReg[i];
        }
};



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


int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned m, n, total_qubits, num_exps;
    unsigned num_classes, numQubits_class;

    m = 4;
    n = 6;
    num_exps = 500;

    total_qubits = 2*n + 2;

    // To store initial patterns 
    vector<unsigned int> pattern(m);
    vector<unsigned int> input_pattern(1);

    pattern[0] = 63;
    pattern[1] = 32;
    pattern[2] = 56;
    pattern[3] = 7;

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits,"base",0);

    // Holds indices of qubits in each register in Circuit
    myRegisters qRegCirc(m,n);

    vector<double> count(m);
    vector<double> prob_dist(m);

    double average = 0;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0,1.0);

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
