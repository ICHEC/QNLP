/**
 * @file bin_into_superpos.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Defines class which introduces routines for encoding binary numbers represented as unsigned integers into a superposition of quantum states. The amplitude of each state will be even. The method used follows the approach outlined in the paper https://arxiv.org/abs/quant-ph/0210176v2 by C. A. Trugenberger. * @version 0.1
 * @date 2019-29-06
 */

#ifndef QNLP_BINENCODE
#define QNLP_BINENCODE

#include <cassert>
#include <utility>
#include <memory>
#include <cmath>
#include<vector>
#include<complex>
#include<iostream>

namespace QNLP{

    /**
     * @brief Checks if the bit'th bit of the integer byte is set
     * 
     * @param byte - integer representing a binary string
     * @param bit - the index of the bit to be checked (beginning
     * with the least significant bit)
     */
    #define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

    /**
     * @brief Definition of class to encode a binary string represented by an integer into a superposition of states. 
     * 
     * @tparam SimulatorType Class simulator type 
     */
    template <class SimulatorType>
    class EncodeBinIntoSuperpos{
        private:
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            std::unique_ptr< std::vector<Mat2x2Type> > S;
            std::size_t m, len_reg_auxiliary, len_bin_pattern;
    
        public:
            /**
             * @brief Construct a new object instance to encode a binary string into a superposition (disabled)
             * 
             */
            EncodeBinIntoSuperpos() = delete;

            /**
             * @brief Construct a new object instance to encode a binary string into a superposition
             * 
             * @param num_bin_patterns Number of binary patterns to encode 
             * @param len_bin_pattern_ Length of the binary patterns being encoded
             */
            EncodeBinIntoSuperpos(const std::size_t num_bin_patterns, const std::size_t len_bin_pattern_){
                m = num_bin_patterns;
                len_bin_pattern = len_bin_pattern_;

                initialiseMats();
            };

            /**
             * @brief Destroy the Encode Bin Into Superpos object
             * 
             */
            ~EncodeBinIntoSuperpos(){
            };

            /**
             * @brief Initialiser of the encoder (define the required PauliX and the unitary matrix S)
             * 
             */
            void initialiseMats(){

                /* Preparation for binary encoding:
                 *
                 * Prepare three matrices S^1,S^2,...,S^m that are required for the implemented
                 * algorithm to encode these binary strings into a superposition state.
                 *
                 * Note the matrix indexing of the S vector of S^p matrices will be backwards:
                 *      S[0] -> S^p
                 *      S[1] -> S_{p-1}, and so on.
                 */
                S.reset(new std::vector<Mat2x2Type> (m));
                {
                    int p = m;
                    double diag, off_diag;

                    for(std::size_t i = 0; i < m; i++){
                        off_diag = 1.0/sqrt((double)(p));
                        diag = off_diag * sqrt((double)(p-1));

                        (*S)[i](0,0) = {diag, 0.0};
                        (*S)[i](0,1) = {off_diag, 0.0};
                        (*S)[i](1,0) = {-off_diag, 0.0};
                        (*S)[i](1,1) = {diag, 0.0};

                        p--;
                    }
                }
            }

            /**
             * @brief Encodes each element of inputted vector as a binary string in a superpostiion of states. Requires each binary input to be unique
             * 
             * @param qReg Qubit register
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_auxiliary A vector containing the indices of the qubits of the auxiliary register. 
             * @param Vector of non-negative integers which represent the inputted binary patters that are to be encoded.
             */
            void encodeBinInToSuperpos_unique(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_auxiliary, 
                    const std::vector<std::size_t>& bin_patterns){
                std::size_t len_reg_auxiliary = reg_auxiliary.size();

                // Require length of auxiliary register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_auxiliary);
                // Prepare state in |0...>|0...0>|01> of lengths n,n,2
                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("Prepare state in |0...>|0...0>|01> of lengths n,n,2");
                #endif
                qSim.applyGateX(reg_auxiliary[len_reg_auxiliary-1]);
                std::vector<std::size_t> sub_reg(reg_memory.begin(), reg_memory.begin () + len_bin_pattern);

                // Iteratively encode each binary pattern.
                for(std::size_t i = 0; i < m; i++){

                    // Psi0
                    // Encode inputted binary pattern.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_auxiliary[j]);
                        }
                    }

                    // Psi1
                    // Copy pattern to auxiliary register of newly created state (now becoming the `active` state).
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_1 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCCX(reg_auxiliary[j], reg_auxiliary[len_reg_auxiliary-1], reg_memory[j]);
                    }

                    // Psi2
                    // Set memory register to state |11..1> if it is the active state.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_2 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCX(reg_auxiliary[j], reg_memory[j]);
                        qSim.applyGateX(reg_memory[j]);
                    }

                    // Psi3
                    // Apply NCU to flip qubit in second auxiliary register (index `len_reg_auxiliary-2`).
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_3 \\rangle");
                    #endif
                    std::vector<std::size_t> tmp_aux(reg_auxiliary.begin(), reg_auxiliary.begin() + sub_reg.size()-2 );
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, tmp_aux, reg_auxiliary[len_reg_auxiliary-2], "X");

                    // Psi4
                    // Apply S^i
                    // This flips the second control bit of the new term in the position so
                    // that we get old|11> + new|01> thus breaking it off into larger and smaller chunks.
                    // The new state is now defined as the next 'newly created state'.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_4 \\rangle");
                    #endif
                    qSim.applyGateCU((*S)[i], reg_auxiliary[len_reg_auxiliary-2], reg_auxiliary[len_reg_auxiliary-1]);


                    // Psi5
                    // Uncompute NCU
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_5 \\rangle");
                    #endif
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, tmp_aux, reg_auxiliary[len_reg_auxiliary-2], "X");                    

                    // Psi6 
                    // Uncompute setting of memory register to all 1's of active state.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_6 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                        qSim.applyGateX(reg_memory[j]);
                        qSim.applyGateCX(reg_auxiliary[j], reg_memory[j]);
                    }

                    // Psi7
                    // Uncompute encoding.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_7 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                       qSim.applyGateCCX(reg_auxiliary[j], reg_auxiliary[len_reg_auxiliary-1], reg_memory[j]);
                    }

                    // Reset the register of the new term to the state |m>|0...0>|01>.
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("Reset p to | 00\\ldots 0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        // Check current pattern against next pattern
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_auxiliary[j]);
                        }
                    }
                }
            }
    };

};
#endif
