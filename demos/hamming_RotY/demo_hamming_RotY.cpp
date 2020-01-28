/**
 * @file demo_hamming_RotY.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Compute the similarity of a test state to a set of training states using a Hamming distance metric to execute a Y-rotation for about some angle \theta for each similar binary pattern. A superposition of training states are initially encoded, the Hamming distance is computed and Y-rotations executed on an auxiliary qubit to proportionally adjust the amplitudes corresponding to the similarity of each training state to the test state. A single state is then measured. This is repeated to build a distribution. 
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

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

    // Default config
    bool verbose = false;
    std::size_t len_reg_memory = 2;
    std::size_t num_exps = 1000;
    std::size_t test_pattern = 3;

    if(argc > 1){
        verbose = atoi(argv[1]);
    }
    else if(argc > 2){
        test_pattern = atoi(argv[2]);
        if(test_pattern < 0){
            std::cerr << "Error: Test Patter is negative. Must be positive integer." << std::end;
        }
    }
    else if(argc > 3){
        num_exps = atoi(argv[2]);
    }
    else if(argc > 4){
        len_reg_memory = atoi(argv[3]);
    }
    else{
        std::cerr << "Error: " << argc << " arguments supplied, expected 1, 2, 3 or 4 (verbosity [bool], test pattern [unsigned integer], number of shots [unsigned integer], length of binary states to encode [unsigned integer]). " << std::endl;
    }

    len_reg_auxiliary = len_reg_memory + 2;
    num_qubits = len_reg_memory + len_reg_auxiliary;;
    num_bin_pattern = pow(2,len_reg_memory);

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
        #ifdef GATE_LOGGING
        sim->getGateWriter().segmentMarkerOut("Encode");
        #endif
        sim->encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory); 

        // Print superposition of states in each experimentation
        if(verbose && rank == 0){
            sim->PrintStates("After encoding: ");
        }

        // Compute Hamming distance between test pattern and encoded patterns
        #ifdef GATE_LOGGING
        sim->getGateWriter().segmentMarkerOut("Compute Hamming distance");
        #endif
        sim->applyHammingDistanceRotY(test_pattern, reg_memory, reg_auxiliary, len_reg_memory);

        // Print superposition of states after the amplitude adjustment
        if(verbose && rank == 0){
            sim->PrintStates("After Hamming Rot_Y: ");
        }

        // Collapse to the Z-basis on the qubit which rotations were computed.
        sim->collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1);

        // Print superposition after collapse to the Z-basis on the qubit which rotations were computed.
        if(verbose && rank == 0){
            sim->PrintStates("After Collapse to Basis z: ");
        }

        // Measure a single state
        val = sim->applyMeasurementToRegister(reg_memory);

        // Print final measured state.
        if(verbose && rank == 0){
            sim->PrintStates("After Measurement: ");
        }

        // Increment count of measured state
        count[val] += 1;
    }

    if(rank == 0){
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
    
