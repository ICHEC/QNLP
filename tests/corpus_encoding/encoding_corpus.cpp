#include "util/tinymatrix.hpp"
#include "read_tags.hpp"

#include "QubitCircuit.hpp"
#include <bitset>

#define __VERBOSE__ 1

//#####################

unsigned int update_left_shift_binary(unsigned int current, bool val){
    return (current << 1) & val;
}

void print_bits(unsigned int val, unsigned int len){
    for (int i = len-1; i > -1; i--){
        std::cout << ((val >> i) & 1UL);
    }
}

void print_all_bit_strings(vector<unsigned int> pattern, unsigned int start, unsigned int end, unsigned int len){
    for(unsigned int i = start; i < end; i++){
        print_bits(pattern[i],len);
    }
}

#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

// Set vectors containing appropriate indices of each qubit in each register in the overall circuit
//      x       - n
//      input   - n
//      c       - m + 1
//      g       - 1
void set_circ_register_indexing(vector<unsigned>& x, vector<unsigned>& input, vector<unsigned>& c, vector<unsigned>& g, unsigned int m, unsigned int n){
    for(unsigned int i = 0; i < n; i++){
        x[i] = i;
    }
    for(unsigned int i = 0; i < n; i++){
        input[i] = i + n;
    }
    for(unsigned int i = 0; i < m + 1; i++){
        c[i] = i + 2*n;
    }
    g[0] = m + 2*n + 1;
}


void set_circ_register_indexing(vector<unsigned>& x, vector<unsigned>& input, vector<unsigned>& c, vector<unsigned>& g, vector<unsigned>& class_reg, unsigned int m, unsigned int n, unsigned int num_q_class){
    for(unsigned int i = 0; i < n; i++){
        x[i] = i;
    }
    for(unsigned int i = 0; i < n; i++){
        input[i] = i + n;
    }
    for(unsigned int i = 0; i < m + 1; i++){
        c[i] = i + 2*n;
    }
    g[0] = m + 2*n + 1;
    for(unsigned int i = 0; i < num_q_class; i++){
        class_reg[i] = i + m + 2*n + 2;
    }
}

// Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
//              - Let there exist m vectors of binary number, each of length n.
//              - Require m+2n+2 qubits
template < class Type >
void encode_binarystrings(vector<unsigned int> pattern, QubitCircuit<Type>& circ, unsigned int m, unsigned int n){

    // Use vectors to store indices of appropriate registers in circuit 
    //      - x holds superposition of training data
    //      - c is an auxiliaryry control register
    //      - g is garbage register - also named auxiliary qubit
    vector<unsigned> x(n);
    vector<unsigned> dummy(n);
    vector<unsigned> c(m+1);
    vector<unsigned> g(1);

    set_circ_register_indexing(x,dummy,c,g,m,n);

    // We wil then prepare three matrices S^1,S^2,...,S^3 that are required for the implemented
    // algorithm to encode these binary strings into a superposition state.
    //
    // Note the matrix indexing of the S vector of S^p matrices will be backwards:
    //      S[0] -> S^p
    //      S[1] -> S_{p-1}, and so on.
    //
    vector<openqu::TinyMatrix<Type, 2, 2, 32>> S(m);
    {
        unsigned int p = m;
        double diag, off_diag;

        for(unsigned int i = 0; i < m; i++){
            off_diag = 1.0/sqrt((double)(p));
            diag = off_diag * sqrt((double)(p-1));

            S[i](0,0) = {diag, 0.0};
            S[i](0,1) = {off_diag, 0.0};
            S[i](1,0) = {-off_diag, 0.0};
            S[i](1,1) = {diag, 0.0};

            p--;
        }
    }

    // Prepare state in |...>|10...>|0>
    circ.ApplyPauliX(c[0]);

    circ.ApplyCPauliX(c[0],c[1]);

    // Encode inputted binary pattern
    for(unsigned int j = 0; j < n; j++){
        if(IS_SET(pattern[0],j)){
            circ.ApplyCPauliX(c[1],x[j]);
        }
    }

    // Step 1.3 - Apply S^i
    // This flips the second control bit of the new term in the position so
    // that we get old|11> + new|10>
    // Break off into larger and smaller chunks
    circ.ApplyControlled1QubitGate(c[0], c[1], S[0]);

    for(unsigned int i = 1; i < m; i++){
        circ.ApplyCPauliX(c[i],c[i+1]);
        // Encode inputted binary pattern
        for(unsigned int j = 0; j < n; j++){

            if(IS_SET(pattern[i-1],j)){
                circ.ApplyCPauliX(c[i+1],x[j]);
            }
            if(IS_SET(pattern[i],j)){
                circ.ApplyCPauliX(c[i+1],x[j]);
            }
        }

        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        circ.ApplyControlled1QubitGate(c[i], c[i+1], S[i]);
    }
}

//####################

int main(int argc, char **argv){

    //Initialise qHiPSTER MPI environment
    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned num_patterns=0, pattern_length=0, total_qubits, num_exps;


    //Number of experimental shots taken
    num_exps = 10;

    //Declare database tag reader and load the respective datasets from SQLite
    QNLP::ReadTags rt {};
    rt.loadData("noun");
    rt.loadData("verb");

    //Number of patterns to encode will be equal to the number of unique entries loaded from the database
    num_patterns = rt.getNameToBin()["noun"].size();
    QNLP::ReadTags::NTB& type_map = rt.getNameToBin();
    std::cout << "SIZE=" << num_patterns << std::endl;

    //The required qubit register length to represent the patterns will be given by the log2 of the number of patterns
    pattern_length = (int) std::ceil(std::log2(num_patterns));
    total_qubits = num_patterns + 2*pattern_length + 1 + 1;
    std::cout << "QUBITS=" << total_qubits << std::endl;

    vector<unsigned int> pattern(num_patterns);
    cout << "PATTERN_SIZE" << pattern.size() << endl;

    unsigned int ct = 0;
    for (auto& name_bin : type_map["noun"] ){
        pattern[ct] = name_bin.second;
        std::cout << "NOUN: Name=" << name_bin.first << "\t\t Pattern=" << name_bin.second << std::endl;
        ct++;
    }
    //rt.closeDB();

    // To store initial patterns 
    vector<unsigned int> input_pattern(1);
    input_pattern[0] = 3;
    std::cout << "PATTERN LENGTH=" << pattern_length << std::endl;

    vector<unsigned int> all_pattern(pow(2,pattern_length));
    for(unsigned int i = 0; i < pow(2,pattern_length); i++){
        all_pattern[i] = i;
    }


    // Use vectors to store indices of appropriate registers in circuit 
    //      - x holds superposition of training data
    //      - c is an auxiliaryry control register
    //      - g is garbage register - also named auxiliary qubit
    vector<unsigned int> x(pattern_length);
    vector<unsigned int> input(pattern_length);
    vector<unsigned int> c(num_patterns+1);
    vector<unsigned int> g(1);

    set_circ_register_indexing(x, input, c, g, num_patterns, pattern_length);

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits);

    vector<double> count(num_patterns);

    unsigned int val;

    double p1, p2;
    int auxiliary, count_auxiliary_is_one;
    count_auxiliary_is_one = 0;

    unsigned int exp = 0;

    while(exp++ < num_exps){
        circ.Initialize("base",0);
        std::cout << "EXP. SHOT: " << exp << std::endl;

        // Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
        //              - Let there exist m vectors of binary number, each of length n.
        //
        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(pattern, circ, num_patterns, pattern_length);

        // Step 2      - Compute the Hamming distance between the input pattern and each training pattern.
        //             - Results are stored in the coefficient of each state of the input pattern
        //compute_HammingDistance<ComplexDP>( pattern, input_pattern, circ,  num_patterns,  pattern_length);

        // If auxiliary collapses to state |1> we discard this experiment
        circ.ApplyMeasurement(g[0]);
        auxiliary = circ.GetProbability(g[0]);

        // Reject sample
        if(auxiliary){
            count_auxiliary_is_one++;
        }
        // Accept sample
        else{
            // Collapse qubits in input register
            for(unsigned int j = 0; j < pattern_length; j++){
                circ.ApplyMeasurement(x[j]);
            }

            // Store current state of training register in it's integer format
            val = 0;
            for(unsigned int j = pattern_length-1; j > -1; j--){
                val |= ((unsigned int) circ.GetProbability(x[j]) << j);
            }

            // Increase the count of the training pattern measured
            bool flag = false;
            for(unsigned int i = 0; i < num_patterns; i++){
                if(pattern[i] == val){
                    count[i]++;
                    flag = true;
                }
            }
            if(!flag){
                print_bits(val, pattern_length);
                cout << endl;
            }
        }
    }

    cout << "NumTimes auxiliary was one: \t" << count_auxiliary_is_one << endl;

    double dist;
    double prob, prob_sum;
    prob_sum = 0.0;
    cout << "state\t\tfreq\tprob\tdist\tProb(D)" << endl;

    for(unsigned int i = 0; i < num_patterns; i++){

        prob = count[i]/(double) num_exps;
        prob_sum += prob;
        dist = (2.*pattern_length)*acos(sqrt(prob))/M_PI;

        cout << i << " |";
        print_bits( pattern[i], pattern_length );
        cout << ">\t" << count[i] << "\t" << count[i]/(double)num_exps * 100.0 << "%\t" << dist  << "\t"<< dist / (double) pattern_length << endl;
    }
    cout << "Sum of probs =\t" << prob_sum << endl;

    cout << "TestPattern = " << "|";
    print_bits(input_pattern[0], pattern_length);
    cout << ">" << endl;;

    return 0;
}
