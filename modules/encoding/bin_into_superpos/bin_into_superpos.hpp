/**
 * @file bin_into_superpos.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Functions for encoding binary numbers into a superposition of states
 * @version 0.1
 * @date 2019-29-06
 */

#ifndef QNLP_BINENCODE
#define QNLP_BINENCODE

//#include "Simulator.hpp"
//#include <complex>
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

    template <class SimulatorType>
    class EncodeBinIntoSuperpos{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            std::unique_ptr< std::vector<Mat2x2Type> > S;
            std::size_t m, len_reg_ancilla, len_bin_pattern;
    
        public:
            EncodeBinIntoSuperpos(){};

            EncodeBinIntoSuperpos(const std::size_t num_bin_patterns, const std::size_t len_bin_pattern_){
                m = num_bin_patterns;
                len_bin_pattern = len_bin_pattern_;

                initialiseMats();
            };

            ~EncodeBinIntoSuperpos(){
                clearMats();    
            };

            /**
             * @brief Define the PauliX and the unitary matrix S
             * 
             */
            //template <class Type>
            void initialiseMats(){

                // Preparation for binary encoding:
                //
                // Prepare three matrices S^1,S^2,...,S^m that are required for the implemented
                // algorithm to encode these binary strings into a superposition state.
                //
                // Note the matrix indexing of the S vector of S^p matrices will be backwards:
                //      S[0] -> S^p
                //      S[1] -> S_{p-1}, and so on.
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
             * @brief Clears the S matricess 
             * 
             */
            void clearMats(){
                //delete  [] S;
                //S.clear();
            }

            /**
             * @brief Encodes each element of inputted vector as a binary string in a superpostiion of states.
             * 
             * @param qReg Qubit register
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_ancilla A vector containing the indices of the qubits of the ancilla register. 
             * @param Vector of non-negative integers which represent the inputted binary patters that are to be encoded.
             */
            void encodeBinInToSuperpos(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_ancilla, 
                    const std::vector<std::size_t>& bin_patterns){

                std::cerr << "NOT YET IMPLEMENTED CORRECTLY" << std::endl;
                std::abort();
                
                std::size_t len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_ancilla);

                // Prepare state in |0...>|0...0>|10> of lengths n,n,2

                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("Prepare state in |0...>|0...0>|10> of lengths n,n,2");
                #endif

                qSim.applyGateX(reg_ancilla[len_reg_ancilla-1]);
                std::vector<std::size_t> sub_reg(reg_memory.begin(), reg_memory.begin () + len_bin_pattern);

                // Begin Encoding
                for(std::size_t i = 0; i < m; i++){
                    // Psi0
                    // Encode inputted binary pattern to pReg
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_ancilla[j]);
                        }
                    }

                    // Psi1
                    // Encode inputted binary pattern
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_1 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Psi2
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_2 \\rangle");
                    #endif                    
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                        qSim.applyGateX(reg_memory[j]);
                    }

                    // Psi3
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_3 \\rangle");
                    #endif
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, reg_ancilla[len_reg_ancilla-2]);


                    // Psi4
                    // Step 1.3 - Apply S^i
                    // This flips the second control bit of the new term in the position so
                    // that we get old|11> + new|10>
                    // Break off into larger and smaller chunks
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_4 \\rangle");
                    #endif
                    qSim.applyGateCU((*S)[i], reg_ancilla[len_reg_ancilla-2], reg_ancilla[len_reg_ancilla-1]);

                    // Psi5
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_5 \\rangle");
                    #endif
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, reg_ancilla[len_reg_ancilla-2]);

                    // Psi6 
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_6 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                        qSim.applyGateX(reg_memory[j]);
                        qSim.applyGateCX(reg_ancilla[j], reg_memory[j]);
                        //qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Psi7
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_7 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                       qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Reset the p register of the new term to the state |0...0>
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("Reset p to | 00\\ldots 0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        // Check current pattern against next pattern
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_ancilla[j]);
                        }

                    }
                }

            }


            /**
             * @brief Encodes each element of inputted vector as a binary string in a superpostiion of states. Requires each binary input to be unique
             * 
             * @param qReg Qubit register
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_ancilla A vector containing the indices of the qubits of the ancilla register. 
             * @param Vector of non-negative integers which represent the inputted binary patters that are to be encoded.
             */
            void encodeBinInToSuperpos_unique(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_ancilla, 
                    const std::vector<std::size_t>& bin_patterns){

                
                std::size_t len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_ancilla);

                // Prepare state in |0...>|0...0>|10> of lengths n,n,2
                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("Prepare state in |0...>|0...0>|10> of lengths n,n,2");
                #endif

                qSim.applyGateX(reg_ancilla[len_reg_ancilla-1]);
                std::vector<std::size_t> sub_reg(reg_memory.begin(), reg_memory.begin () + len_bin_pattern);
                //qSim.applyGateNCU(pX, reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2]);

                // Begin Encoding
                for(std::size_t i = 0; i < m; i++){
                    // Psi0
                    // Encode inputted binary pattern to pReg
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_ancilla[j]);
                        }
                    }

                    // Psi1
                    // Encode inputted binary pattern
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_1 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Psi2
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_2 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        qSim.applyGateCX(reg_ancilla[j], reg_memory[j]);
                        qSim.applyGateX(reg_memory[j]);
                    }

                    // Psi3
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_3 \\rangle");
                    #endif
                    //qSim.applyGateNCU(qSim.getGateX(), sub_reg, reg_ancilla[len_reg_ancilla-2]);
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, reg_ancilla[len_reg_ancilla-2]);
                    //qSim.applyGateNCU(qSim.getGateX(), reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2]);

                    // Psi4
                    // Step 1.3 - Apply S^i
                    // This flips the second control bit of the new term in the position so
                    // that we get old|11> + new|10>
                    // Break off into larger and smaller chunks
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_4 \\rangle");
                    #endif
                    qSim.applyGateCU((*S)[i], reg_ancilla[len_reg_ancilla-2], reg_ancilla[len_reg_ancilla-1]);

                    // Psi5
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_5 \\rangle");
                    #endif
                    qSim.applyGateNCU(qSim.getGateX(), sub_reg, reg_ancilla[len_reg_ancilla-2]);
                    //qSim.applyGateNCU(qSim.getGateX(), reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2]);

                    // Psi6 
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_6 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                        qSim.applyGateX(reg_memory[j]);
                        qSim.applyGateCX(reg_ancilla[j], reg_memory[j]);
                    }

                    // Psi7
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("| \\Psi_7 \\rangle");
                    #endif
                    for(int j = len_bin_pattern-1; j > -1; j--){
                       qSim.applyGateCCX(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Reset the p register of the new term to the state |0...0>
                    #ifdef GATE_LOGGING
                    qSim.getGateWriter().segmentMarkerOut("Reset p to | 00\\ldots 0 \\rangle");
                    #endif
                    for(std::size_t j = 0; j < len_bin_pattern; j++){
                        // Check current pattern against next pattern
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.applyGateX(reg_ancilla[j]);
                        }
                    }
                }
            }
    };

};
#endif


