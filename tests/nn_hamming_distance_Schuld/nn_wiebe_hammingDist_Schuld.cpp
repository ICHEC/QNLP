/**
 * @file nn_wiebe_hammingDist_Schuld.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Encodes binary strings and computes the Hamming distance between test string.
 * @version 0.1
 * @date 2019-04-03
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
 *      - Computes the Hamming distance between a test pattern and all
 *      of the binary strings stored in the superposition, storing the distance
 *      in the corresponding amplitude term.
 * 
 *  NOTE:
 *      This application needs further validation and investigation as the results
 *      are not as expected after the quantum Hamming distance algorithm is applied.
 * 
 *  To Do - Formulate relation with NLP compositional semantics work, implement, apply
 *      decoding of Hamming distance results with fewer classes than training points
 *      correctly (decoding to relative distances only works for each point with a 
 *      unique class label).
 */


/**
 * @brief Class to Update and store the global circuit indices for each register
 * 
 * @param x - Training data register
 * @param input - Input data register
 * @param c - Ancilla register for the encoding stage
 * @param g - Ancilla register for the Hamming distance stage
 * @param class_reg - Register for binary representation of class label 
 * @param m - Number of training datum
 * @param n - Length of training data binary strings
 * @param num_q_class - Number of unique classes
 */
class myRegisters{
    private:
        unsigned m, n, numQubits_class;
        // Use vectors to store indices of appropriate registers in circuit 
        //      - x holds superposition of training data
        //      - input holds input binary string
        //      - class_reg holds class binary string for corresponding training data
        //      - c is an ancillary control register
        //      - g is garbage register - also named ancilla qubit
        vector<unsigned> p;
        vector<unsigned> pClass;
        vector<unsigned> m;
        vector<unsigned> m_class;
        vector<unsigned> c;
        vector<unsigned> g;
        
    public:
        myRegisters(int m_, int n_, int numQubits_class_) : m(m_), n(n_), numQubits_class(numQubits_class_), x(n), input(n), classReg(numQubits_class), classReg2(numQubits_class), c(m+1), g(1){
            
            g[0] = m + n + 1;
            for(int i = 0; i < n; i++){
                m[i] = i + m + n + 2 + 2*numQubits_class_;
            }

            for(int i = 0; i < n; i++){
                p[i] = i + m + n + 2;
            }
            for(int i = 0; i < numQubits_class_; i++){
                p_class[i] = i + m + n + 2;
            }
            for(int i = 0; i < m + 1; i++){
                c[i] = i + n;
            }


            for(int i = 0; i < n; i++){
                m[i] = i;
            }
            for(int i = 0; i < numQubits_class_; i++){
                m_class[i] = i + m + n + 2 + numQubits_class_;
            }

        }

        unsigned get_numTrainStrings(){
            return m;
        }
        unsigned get_lenTrainStrings(){
            return n;
        }
        unsigned get_lenClassesStrings(){
            return numQubits_class;
        }

        void set_x(unsigned i, unsigned val){
            assert(i < n);
            x[i] = val;
        }
        void set_input(unsigned i, unsigned val){
            assert(i < n);
            input[i] = val;
        }
        void set_classReg(unsigned i, unsigned val){
            assert(i < numQubits_class);
            classReg[i] = val;
        }
        void set_c(unsigned i, unsigned val){
            assert(i < m+1);
            c[i] = val;
        }
        void set_g(unsigned i, unsigned val){
            assert(i < 1);
            g[i] = val;
        }

        unsigned get_x(unsigned i){
            assert(i < n);
            return x[i];
        }
        unsigned get_input(unsigned i){
            assert(i < n);
            return input[i];
        }
        unsigned get_classReg(unsigned i){
            assert(i < numQubits_class);
            return classReg[i];
        }
        unsigned get_c(unsigned i){
            assert(i < m+1);
            return c[i];
        }
        unsigned get_g(unsigned i){
            assert(i < 1);
            return g[i];
        }
        unsigned get_classReg2(unsigned i){
            assert(i < numQubits_class);
            return classReg2[i];
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
//              - Require m+2n+2 qubits
/**
 * @brief Updates the circuit by encoding the binary strings of the training data into a superposition represented
 * in the x register. The c register is used as an ancilla register and is unusable after this encoding has been completed.
 * 
 * @tparam Type - datatype of the quantum circuit
 * @param pattern - binary strings in integer format to be encoded
 * @param pattern_class - class labels for respective input patterns which are also encoded
 * @param circ - Quantum circuit
 * @param m - number of training binary strings
 * @param n - length of input binary strings
 * @param numQubits_class - number of unique classes present
 */
/**
 * @brief 
 * 
 * @tparam Type 
 * @param pattern 
 * @param pattern_class 
 * @param circ 
 * @param qReg 
 * @param S 
 * @param op_nCDecomp 
 * @param X 
 */
template < class Type >
void encode_binarystrings(vector<unsigned int>& pattern, vector<unsigned int>& pattern_class, QubitCircuit<Type>& circ, myRegisters& qReg, vector<openqu::TinyMatrix<Type, 2, 2, 32>>& S, NCU<ComplexDP>& op_nCDecomp, openqu::TinyMatrix<Type, 2, 2, 32>& X){

    // Encode
    int m,n;
    m = qReg.get_numTrainStrings();
    n = qReg.get_lenTrainStrings();

    // Prepare state in |0...>|0...0>|10> of lengths n,n,2
    circ.ApplyPauliX(qRegCirc.get_cReg(1));

    for(int i = 0; i < m; i++){

        // Psi0
        // Encode inputted binary pattern
        for(int j = 0; j < n; j++){
            if(IS_SET(pattern[i],j)){
                circ.ApplyCPauliX(qReg.get_c(1),qReg.get_x(j));
            }
        }
        // Encode the class identifier into the class register.
        for(int j = 0; j < qReg.get_lenClassesStrings(); j++){
            if(IS_SET(pattern_class[i],j)){
                circ.ApplyCPauliX(qReg.get_c(1),qReg.get_classReg(j));
            }
        }



        // Psi1
        // Encode the class identifier into the class register.
        for(int j = 0; j < qReg.get_lenClassesStrings(); j++){
            circ.ApplyToffoli(qReg.get_classReg(j), qReg.get_c(1), qReg.get_classReg2(j));
        }
        // Encode inputted binary pattern
        for(int j = 0; j < n; j++){
            circ.ApplyToffoli(qReg.get_x(j), qReg.get_c(1), qReg.get_input(j));
        }

        // Psi2
        for(int j = 0; j < qReg.get_lenClassesStrings(); j++){
            circ.ApplyPauliX(qReg.get_classReg2(j));
            circ.ApplyCPauliX(qReg.get_classReg(j), qReg.get_classReg2(j));
        }
        for(int j = 0; j < n; j++){
            circ.ApplyPauliX(qReg.get_input(j));
            circ.ApplyCPauliX(qReg.get_x(j), qReg.get_input(j));
        }

        cout << i << " NCX 1 - before" << endl;
        // Psi3
        op_nCDecomp.applyNQubitControl(circ, qReg.get_classReg2(0), qReg.get_input(n-1), qReg.get_c(0), X, 0, true);
        cout << "NCX 1 - after" << endl;

        // Psi4
        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        circ.ApplyControlled1QubitGate(qReg.get_c(0), qReg.get_c(1), S[i]);

        cout << "NCX 2 - before" << endl;
        // Psi5
        op_nCDecomp.applyNQubitControl(circ, qReg.get_classReg2(0), qReg.get_input(n-1), qReg.get_c(0), X, 0, true);
        cout << "NCX 2 - after" << endl;

        // Psi6 
        for(int j = n-1; j > -1; j--){
            circ.ApplyCPauliX(qReg.get_x(j), qReg.get_input(j));
            circ.ApplyPauliX(qReg.get_input(j));
        }
        for(int j = qReg.get_lenClassesStrings() - 1; j > -1; j--){
            circ.ApplyCPauliX(qReg.get_classReg(j), qReg.get_classReg2(j));
            circ.ApplyPauliX(qReg.get_classReg2(j));
        }

        // Psi7
        for(int j = n-1; j > -1; j--){
            circ.ApplyToffoli(qReg.get_x(j), qReg.get_c(1), qReg.get_input(j));
        }
        for(int j = qReg.get_lenClassesStrings() - 1; j > -1; j--){
            circ.ApplyToffoli(qReg.get_classReg(j), qReg.get_c(1), qReg.get_classReg2(j));
        }


    

    }
}

/**
 * @brief Updates the quantum circuit so that the amplitudes of each state in the superposition has a cosine term with ancilla g = |0>
 * and a sine term with ancilla g = |1>. The argument of the trigonometric terms for each state in the superposition contians is 2*pi*d/(2n) 
 * where d is the Hamming distance between that state's training bit string and the test bit string.
 * 
 * @tparam Type - datatype of the quantum circuit
 * @param input_pattern - Integer representing bit string of the test point
 * @param circ - Quantum circuit
 * @param m - number of training binary strings
 * @param n - length of input binary strings
 * @param numQubits_class - number of unique classes present
 */
template <class Type>
void compute_HammingDistance(vector<unsigned int>& input_pattern, QubitCircuit<Type>& circ, myRegisters& qReg, vector<openqu::TinyMatrix<Type, 2, 2, 32>>& U){

   int m,n;
    m = qReg.get_numTrainStrings();
    n = qReg.get_lenTrainStrings();

    // Step 2       - Hamming distance calculation
    //
    // Step 2.1      - Encode test pattern into quantum register input
    for(int j = 0; j < n; j++){
        if(IS_SET(input_pattern[0],j)){
            circ.ApplyPauliX(qReg.get_input(j));
        }
    }

    // Apply Hadamard to the Ancilla bit g
    circ.ApplyHadamard(qReg.get_g(0));

    // Step 2.2     
    //              - Apply CX with control the input vector and target the superpositions 
    //              - Apply X to all x values
    for(int j = 0; j < n; j++){
        circ.ApplyCPauliX(qReg.get_input(j),qReg.get_x(j));
        circ.ApplyPauliX(qReg.get_x(j));
    }

// -- CHECKED ABOVE - correct

    // Step 2.3     - Compute Hamming distance with the distances recorded in the input register

    // Apply unitary with H=Identiy to the test data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(qReg.get_input(j),U[0]);
    }

    // Apply unitary with H=PauliZ to the ancillary register
    circ.Apply1QubitGate(qReg.get_g(0),U[1]);


    // Apply unitary with H=(PauliZ+1)/2 to the input data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(qReg.get_x(j),U[2]);
    }

    //  ???
    // Apply unitary with H=Identiy to the ancillary junk data registers
    for(int j = 0; j < m+1; j++){
          circ.Apply1QubitGate(qReg.get_c(j),U[0]);
    }

    // Apply unitary with H=Identiy to the training class registers
    for(int j = 0; j < qReg.get_lenClassesStrings(); j++){
          circ.Apply1QubitGate(qReg.get_classReg(j),U[0]);
    }

    // Step 2.4     - Apply Hadamard to ancilla bit again
    circ.ApplyHadamard(qReg.get_g(0));
    // Try doing hadamard but in exponentil term with identity for all others!!


}





int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned m, n, total_qubits, num_exps;
    unsigned num_classes, numQubits_class;

    m = 8;
    n = 5;
    num_exps = 100;
    num_classes = 4;

    numQubits_class = (int)ceil(log2(num_classes));
    total_qubits = m + 2*n + 1 + 1 + 2*numQubits_class;

    // To store initial patterns 
    vector<unsigned int> pattern(m);
    vector<unsigned int> input_pattern(1);
    vector<unsigned int> pattern_class(m);

    pattern[0] = 4;
    pattern[1] = 16;
    pattern[2] = 31;
    pattern[3] = 30;
    pattern[4] = 5;
    pattern[5] = 6;
    pattern[6] = 11;
    pattern[7] = 13;

    pattern_class[0] = 0;
    pattern_class[1] = 0;
    pattern_class[2] = 1;
    pattern_class[3] = 1;
    pattern_class[4] = 2;
    pattern_class[5] = 2;
    pattern_class[6] = 3;
    pattern_class[7] = 3;

    input_pattern[0] = 30;

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits,"base",0);

    // Holds indices of qubits in each register in Circuit
    myRegisters qReg(m,n,numQubits_class);

    vector<double> count(m);
    vector<double> prob_class(m);

    unsigned int val;

    double p1, p2;
    bool ancilla;
    int count_ancilla_is_one;
    count_ancilla_is_one = 0;


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



    double dart, partition;
    unsigned partition_id;


    int exp = 0;
    while(exp < num_exps){
        average = 0.0;
        circ.Initialize("base",0);

        // Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
        //              - Let there exist m vectors of binary number, each of length n.
        //
        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(pattern, pattern_class, circ, qReg, S, op_nCDecomp,X);


        // Step 2      - Compute the Hamming distance between the input pattern and each training pattern.
        //             - Results are stored in the coefficient of each state of the input pattern
        compute_HammingDistance<ComplexDP>(input_pattern, circ, qReg, U);

        // If ancilla collapses to state |1> we discard this experiment
        ancilla = (dist(mt) < circ.GetProbability(qReg.get_g(0)));
        circ.CollapseQubit(qReg.get_g(0),ancilla);
        //circ.Normalize();

        // Reject sample
        if(ancilla){
            count_ancilla_is_one++;
        }
        // Accept sample
        else{

            for(vector<double>::iterator it = prob_class.begin(); it != prob_class.end(); it++){
                *it = 1.0;
            }


            // Obtains the probability of each state being measured
            for(int i = 0; i < num_classes; i++){
                for(int j = 0; j < numQubits_class; j++){

                    dart = dist(mt);
                    // Multiply prob_class by the probability observed for that qubit being the same as that of the class pattern qubit
                    prob_class[i] *= (!IS_SET(pattern_class[i],j))*(1.0 - circ.GetProbability(qReg.get_classReg2(j))) +  (IS_SET(pattern_class[i],j))*(circ.GetProbability(qReg.get_classReg2(j)));
                }
            }

            partition = prob_class[0];
            partition_id = 0;

            while(dart > partition){
                partition_id++;
                partition += prob_class[partition_id];
            }
#define DEBUG
#ifdef DEBUG
            cout << "\t" << prob_class[0] << "\t"; 
            for(int i = 1; i < num_classes; i++){
                cout << prob_class[i] << "\t"; 
            }
            cout << partition_id << "   partition=" << partition <<  endl;
#endif
            assert(partition_id < num_classes);
            assert(partition - 1.0 <= EPSILON);

            // Collapse qubits to state randomly selectd in class register
            for(int j = 0; j < numQubits_class; j++){
                circ.CollapseQubit(qReg.get_classReg(j),IS_SET(pattern_class[partition_id],j));
            }
            circ.Normalize();

            // Increase the count of the training pattern measured
            count[partition_id]++;

            exp++;
        }
    }

    if(rank == 0){
        cout << "NumTimes ancilla was one (ie sine term was measured): \t" << count_ancilla_is_one << endl;
        cout << "------ Training ------" << endl;
        for(int i = 0; i < m; i++){
            print_bits(pattern[i],n);
            cout << "\tClass: " << pattern_class[i] << " - ";  
            print_bits(pattern_class[i], numQubits_class);
            cout << endl;
        }

        double dist;
        double prob, prob_sum, prob_cos_term;
        prob_sum = 0.0;
        cout << " \tclass\t\tfreq\tprob" << endl;
        prob_cos_term = (double)num_exps/(double)(num_exps + count_ancilla_is_one);
        for(int i = 0; i < num_classes; i++){

            prob = count[i]/(double) num_exps;
            prob_sum += prob;

            cout << i << " \t|";
            print_bits(pattern_class[i], numQubits_class);
            cout << ">\t\t" << count[i] << "\t" << count[i]/(double)num_exps * 100.0 << "%" << endl;
        }
        cout << "Sum of probs =\t" << prob_sum << endl;


//        unsigned chosen_class = max_element(count, count+num_classes);
        unsigned chosen_class = std::distance(count.begin(), std::max_element(count.begin(), count.end()));
;

        cout << "TestPattern = " << "|";
        print_bits(input_pattern[0], n);
        cout << ">" << endl;
        cout << "Pattern Class with highest probability = " << chosen_class << " = |";
        print_bits(pattern_class[chosen_class], numQubits_class);
        cout << ">" << endl;
    }

    return 0;

}

