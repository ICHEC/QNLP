/**
 * @brief Encode a set of unique binary patterns into a superposition, alter their amplitudes according to their similarity with a test binary patter, and get a distribution of the probabilities of these amplitudes.
 *
 */

#include "IntelSimulator.cpp"  
#include "Simulator.hpp"  
#include <map>
#include <iterator>

using namespace QNLP;

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

    bool verbose = false;

    if(argc > 1){
        verbose = atoi(argv[1]);
    }
    std::size_t num_exps = 1000;

    openqu::mpi::Environment env(argc, argv);
    int rank = env.rank();


    std::size_t len_reg_memory = 4;
    std::size_t len_reg_ancilla = len_reg_memory + 2;
    std::size_t num_qubits = len_reg_memory + len_reg_ancilla;;
    std::size_t num_bin_pattern = pow(2,len_reg_memory);

    std::size_t test_pattern = 7;

    if(argc > 2){
        test_pattern = atoi(argv[2]);
    }

    if(argc > 3){
        num_exps = atoi(argv[3]);
    }

    SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(num_qubits);

    // Set up registers to store indices
    std::vector<std::size_t> reg_memory(len_reg_memory);
    for(std::size_t i = 0; i < len_reg_memory; i++){
        reg_memory[i] = i;
    }
    std::vector<std::size_t> reg_ancilla(len_reg_ancilla);
    for(std::size_t i = 0; i < len_reg_ancilla; i++){
        reg_ancilla[i] = i + len_reg_memory;
    }

    // Init data to encode
    std::vector<std::size_t> vec_to_encode(num_bin_pattern);
    for(std::size_t i = 0; i < num_bin_pattern; i++){
        vec_to_encode[i] = i;
    }

    std::size_t val;

    // Init result counter
    std::map<std::size_t, std::size_t> count;
    for(std::size_t i = 0; i < num_bin_pattern; i++){
        count.insert(pair<std::size_t, std::size_t>(vec_to_encode[i],0));
    }


    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        sim->initRegister();

        // Encode
        sim->encodeBinToSuperpos_unique(reg_memory, reg_ancilla, vec_to_encode, len_reg_memory); 

        // Compute Hamming distance between test pattern and encoded patterns
        sim->applyHammingDistance(test_pattern, reg_memory, reg_ancilla, len_reg_memory);

        // Measure
        val = sim->applyMeasurementToRegister(reg_memory);

        count[val] += 1;
        if(verbose){
            // Output resulting state for this experiment
            cout << val << "\t";
            cout << "|";
            print_bits(val, len_reg_memory);
            cout << ">" << endl;
        }
    }
            
    cout << "Expected results for an even distribution: " << 1.0/ (double)num_bin_pattern << endl; 
    cout << "Measure:" << endl;
    int i = 0;
    for(map<std::size_t, std::size_t>::iterator it = count.begin(); it !=count.end(); ++it){
        cout << vec_to_encode[i] << "\t";
        cout << it->first << "\t";
        cout << "|";
        print_bits(it->first, len_reg_memory);
        cout << ">\t";
        cout << it->second << "\t" << ((double) it->second / (double) num_exps) << endl;
        i++;
    }

    return 0;
}
    
