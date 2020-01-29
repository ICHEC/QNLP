/**
 * @file demo_encoding.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Demo of encoding a set of bitstrings represented by integer std::size_t types into a superposition of states. Subsequent measurement of those states shows the equal distribution of the probability of measuring each state. 
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
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

    // Default config
    bool verbose = false;
    std::size_t len_reg_memory = 4;
    std::size_t num_exps = 1000;

    if(argc > 1){
        verbose = atoi(argv[1]);
    }
    else if(argc > 2){
        num_exps = atoi(argv[2]);
    }
    else if(argc > 3){
        len_reg_memory = atoi(argv[3]);
    }
    else{
        std::cerr << "Error: " << argc << " arguments supplied, expected 1, 2, or three (verbosity [bool], number of shots [unsigned integer], length of binary states to encode [unsigned integer]). " << std::endl;
    }

    // Set up length of each quantum register.
    std::size_t len_reg_auxiliary = len_reg_memory + 2;
    std::size_t num_qubits = len_reg_memory + len_reg_auxiliary;;
    std::size_t num_bin_pattern = pow(2,len_reg_memory);    // It is possible to encode less binary patters if desired.

    SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(num_qubits);

    int rank;
#if ENABLE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
    rank = 0;
#endif

    // Set up vectors to store indices within general register
    std::vector<std::size_t> reg_auxiliary(len_reg_auxiliary);
    for(std::size_t i = 0; i < len_reg_auxiliary; i++){
        reg_auxiliary[i] = i;
    }
    std::vector<std::size_t> reg_memory(len_reg_memory);
    for(std::size_t i = 0; i < len_reg_memory; i++){
        reg_memory[i] = i + len_reg_auxiliary;
    }

    // Init data to encode
    std::vector<std::size_t> vec_to_encode(num_bin_pattern);
    for(std::size_t i = 0; i < num_bin_pattern; i++){
        vec_to_encode[i] = i;
    }

    std::size_t val;

    // Init counter to store distribution of measured states.
    std::map<std::size_t, std::size_t> count;
    for(std::size_t i = 0; i < num_bin_pattern; i++){
        count.insert(pair<std::size_t, std::size_t>(vec_to_encode[i],0));
    }

    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        // Re-initialise register before each experiemnt
        sim->initRegister();

        // Encode binary vectors
        sim->encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory); 

        // Measure a state
        val = sim->applyMeasurementToRegister(reg_memory);

        // Increment count of measured state
        count[val] += 1;

        // Print measured state in each experimentation
        if(verbose && rank == 0){
            // Output resulting state for this experiment
            cout << val << "\t";
            cout << "|";
            print_bits(val, len_reg_memory);
            cout << ">" << endl;
        }
    }
            
    // Print results.
    if(rank == 0){
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
    }

    return 0;
}
    
