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

    // Define Memory and auxiliary registers by setting their indices
    // into a corresponding vector
    vector<unsigned> reg_memory(n);
    vector<unsigned> reg_auxiliary(n+2);
    for(int j = 0; j < n; j++){
        reg_memory[j] = j;
    }
    for(int j = 0; j < n+2; j++){
        reg_auxiliary[j] = j + n;
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
        circ.EncodeBinInToSuperposition(reg_memory, reg_auxiliary, pattern, n);

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

