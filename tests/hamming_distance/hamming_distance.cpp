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

class progressBar{
    private:
        unsigned count = 0;
        unsigned num_exps;
        unsigned step;
        unsigned next_partition;

    public:
        progressBar(unsigned num_exps_) : num_exps(num_exps_){
            step = (double) num_exps / 100.0;
            next_partition = step;
            cout << "|" <<  flush;
        }

        void iterate(){
            if(count == next_partition){
                cout << "-" << flush;
                next_partition += step;
                if(next_partition >= num_exps){
                    cout << "|" << endl;
                }
            }
            count++;
        }
};


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

/**
 * @brief Updates the quantum circuit so that the amplitudes of each state in the superposition has a cosine term with ancilla g = |0>
 * and a sine term with ancilla g = |1>. The argument of the trigonometric terms for each state in the superposition contians is 2*pi*d/(2n) 
 * where d is the Hamming distance between that state's training bit string and the test bit string.
 * 
 * @tparam Type - datatype of the quantum circuit
 * @param test_pattern - Integer representing bit string of the test point
 * @param circ - Quantum circuit
 * @param m - number of training binary strings
 * @param n - length of input binary strings
 * @param numQubits_class - number of unique classes present
 */
template <class Type>
void compute_HammingDistance(vector<unsigned int>& test_pattern, QubitCircuit<Type>& circ, myRegisters& qRegCirc, vector<openqu::TinyMatrix<Type, 2, 2, 32>>& U){

   int m,n;
    m = qRegCirc.get_numTrainStrings();
    n = qRegCirc.get_lenTrainStrings();

    // Step 2       - Hamming distance calculation
    //
    // Step 2.1      - Encode test pattern into quantum register input
    for(int j = 0; j < n; j++){
        if(IS_SET(test_pattern[0],j)){
            circ.ApplyPauliX(qRegCirc.get_pReg(j));
        }
    }

    // Apply Hadamard to the Ancilla bit g
    circ.ApplyHadamard(qRegCirc.get_cReg(0));

    // Step 2.2     
    //              - Apply CX with control the input vector and target the superpositions 
    //              - Apply X to all x values
    for(int j = 0; j < n; j++){
        circ.ApplyCPauliX(qRegCirc.get_pReg(j),qRegCirc.get_mReg(j));
        circ.ApplyPauliX(qRegCirc.get_mReg(j));
    }

    // Step 2.3     - Compute Hamming distance with the distances recorded in the input register

    // Apply unitary with H=Identiy to the test data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(qRegCirc.get_pReg(j),U[0]);
    }

    // Apply unitary with H=PauliZ to the ancillary register
    circ.Apply1QubitGate(qRegCirc.get_cReg(0),U[1]);


    // Apply unitary with H=(PauliZ+1)/2 to the input data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(qRegCirc.get_mReg(j),U[2]);
    }

    //  ???
    // Apply unitary with H=Identiy to the ancillary junk data registers
    circ.Apply1QubitGate(qRegCirc.get_cReg(1),U[0]);


    // Step 2.4     - Apply inverse of Step 2.2
    //                        - Apply CX with control the input vector and target the superpositions 
    //                        - Apply X to all x values
    for(int j = 0; j < n; j++){
        circ.ApplyPauliX(qRegCirc.get_mReg(j));
        circ.ApplyCPauliX(qRegCirc.get_pReg(j),qRegCirc.get_mReg(j));
    }



    // Step 2.4     - Apply Hadamard to ancilla bit again
    circ.ApplyHadamard(qRegCirc.get_cReg(0));
}



int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned m, n, total_qubits, num_exps;
    unsigned num_classes, numQubits_class;

    m = 4;
    n = 6;
    num_exps = 400;

    if(argc > 1){
        num_exps = atoi(argv[1]);
    }



    total_qubits = 2*n + 2;

    // To store initial patterns 
    vector<unsigned int> pattern(m);
    vector<unsigned int> test_pattern(1);

    pattern[0] = 63;
    pattern[1] = 32;
    pattern[2] = 56;
    pattern[3] = 7;


    test_pattern[0] = 7;



    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits,"base",0);

    // Holds indices of qubits in each register in Circuit
    myRegisters qRegCirc(m,n);

    vector<vector<double>> count(2, vector<double> (m));
    vector<vector<double>> prob_dist(2, vector<double> (m));


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


    // Preparation for Hamming distance computation:
    // Define unitary matrices U[i] corresponding to the appropriate Hamiltomian that is to be
    // applied.
    vector<openqu::TinyMatrix<ComplexDP, 2, 2, 32>> U(3);
    {
        double cos_term, sin_term, pi_term;


        // U for H=Idnetity
        pi_term = 0.5*M_PI/(double)n;
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[0](0,0) = {cos_term,     -sin_term};
        U[0](0,1) = {0.0,          0.0};
        U[0](1,0) = {0.0,          0.0};
        U[0](1,1) = {cos_term,     -sin_term};

        // U for H=PauliZ
        U[1](0,0) = {cos_term,     -sin_term};
        U[1](0,1) = {0.0,          0.0};
        U[1](1,0) = {0.0,          0.0};
        U[1](1,1) = {cos_term,     sin_term};

        // U for H=(PauliZ +1)/2
        pi_term *= 0.5;
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[2](0,0) = {2.0*cos_term*cos_term - 1.0,     -2.0*cos_term*sin_term};
        U[2](0,1) = {0.0,          0.0};
        U[2](1,0) = {0.0,          0.0};
        U[2](1,1) = {1.0,          0.0};
    }         



    progressBar progress(num_exps);

    double dart;
    bool ancilla_is_set;

    unsigned count_ancilla_is_one = 0;

    for(int exp = 0; exp < num_exps; exp++){
    
        // Re-init
        circ.Initialize("base",0);

        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(pattern, circ, qRegCirc, S, op_nCDecomp,X);

        // Hamming Diatance:
        //             - Compute the Hamming distance between the input pattern and each training pattern.
        //             - Results are stored in the coefficient of each state of the input pattern
        compute_HammingDistance<ComplexDP>(test_pattern, circ, qRegCirc, U);

        // Collapse ancilla register to 0 or 1 randomly
        ancilla_is_set = (dist(mt) < circ.GetProbability(qRegCirc.get_cReg(0)));
        circ.CollapseQubit(qRegCirc.get_cReg(0),ancilla_is_set);
        circ.Normalize();

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
        for(int i = 0; i < m; i++){
            if(pattern[i] == val){
                count[ancilla_is_set][i]++;
            }
        }

        // If ancilla collapses to state |1> we
        // keep track of the number of times this happens.
        count_ancilla_is_one += ancilla_is_set;

        progress.iterate();
    }

    if(rank == 0){
        double dist;
        double prob, prob_sum, prob_cos_term, prob_ancilla_isZero;

        prob_sum = 0.0;
        prob_ancilla_isZero = 1.0 - (double)( count_ancilla_is_one) / (double) num_exps;



        cout << " \tPattern\t\t\tfreq\tprob\tdist" << endl;
        cout << "Cosine (Ancilla = 0)" << endl;
        for(int i = 0; i < m; i++){

            //prob = count[0][i]/(double) (num_exps - count_ancilla_is_one);
            prob = count[0][i]/(double) (num_exps);
            prob_sum += prob;

            dist = 2*n*M_1_PI*acos(m*prob*prob_ancilla_isZero);

            cout << i << " \t|";
            print_bits(pattern[i], n);
            cout << ">\t\t" << count[0][i] << "\t" << prob * 100.0 << "%\t" << dist << endl;
        }
        cout << "Sine (Ancilla = 1)" << endl;
        for(int i = 0; i < m; i++){

            //prob = count[1][i]/(double) count_ancilla_is_one;
            prob = count[1][i]/(double)num_exps;
            prob_sum += prob;

            cout << i << " \t|";
            print_bits(pattern[i], n);
            cout << ">\t\t" << count[1][i] << "\t" << prob * 100.0 << "%" << endl;
        }


        cout << "Sum of probs =\t" << prob_sum << endl;
        cout << "NumTimes ancilla was one (ie sine term was measured): \t" << count_ancilla_is_one << endl << endl;

        cout << "Number of patterns: " << m << endl;
        cout << "Pattern length: " << n << endl;
        cout << "Number of required qubits: " << total_qubits << endl << endl;


        unsigned chosen_pattern_id = std::distance(count[0].begin(), std::max_element(count[0].begin(), count[0].end()));
;
        cout << "TestPattern = " << "|";
        print_bits(test_pattern[0], n);
        cout << ">" << endl;
        cout << "Pattern Class with highest probability = " << chosen_pattern_id << " = |";
        print_bits(pattern[chosen_pattern_id], n);
        cout << ">" << endl;




    }

    return 0;

}
