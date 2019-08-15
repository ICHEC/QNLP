/**
 * @file oracle.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Functions for applying black-box like functions to select appropriate qubits matching given patterns
 * @version 0.1
 * @date 2019-07-08
 */

 //# Phase oracle acts on n, boolean oracle acts on n+1; https://arxiv.org/pdf/1703.10535.pdf for examples

#ifndef QNLP_ORACLE
#define QNLP_ORACLE

#include <complex>
#include <cassert>
#include <vector>
#include <iostream>

namespace QNLP{
    template <class SimulatorType>
    class Oracle{
        private:
        //Take the 2x2 matrix type from the template SimulatorType
        using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

        public:
        Oracle(){};
        ~Oracle(){};

        /**
         * @brief If the qubits in the control lines are not adjacent, temporarily move them to allow NCU. Consider moving this to NCU class and handle non adjacent control lines there.
         * 
         * @param qubitIdx0 
         * @param qubitIdx1 
         */
        void moveQubits(SimulatorType& s, std::size_t qubitIdx0, std::size_t qubitIdx1){
            //To be implemented
        }

        /**
         * @brief Takes bitstring as the binary pattern and indices as the qubits to operate upon. Applies the appropriate PauliX gates to the control lines to call the NCU with the given matrix
         * 
         * @param s 
         * @param bitstring 
         * @param ctrl_indices 
         * @param U Unitary matrix to apply 
         * @return decltype(auto) 
         */
        void bitStringNCU(SimulatorType& s, std::size_t bitstring, const std::vector<std::size_t>& ctrl_indices, const std::size_t target, const Mat2x2Type& U){
            std::size_t bitmask = 0b1;
            std::vector<std::size_t> reverse_pattern;
            for(std::size_t i = 0; i < ctrl_indices.size() + 1; ++i){
                //std::cout << "CONTROL=" << i << "   BITSTRING=" << bitstring << "   BITMASK=" << (bitmask<<i) << "  BITSTRING & BITMASK=" << (bitstring & (bitmask<<i) ) <<std::endl;
                //If the bitstring contains a 1 at desired index, will be true;
                //We wish to apply X to any state that is false, then undo
                if( ! (bitstring & (bitmask<<i) ) ){
                    reverse_pattern.push_back(i);
                    if( i < ctrl_indices.size())
                        s.applyGateX(ctrl_indices[i]);
                    else
                        s.applyGateX(target);
                }
            }
            s.applyGateNCU(U, ctrl_indices.front(), ctrl_indices.back(), target);
            //Undo PauliX ops
            for(auto& revIdx : reverse_pattern){
                if( revIdx < ctrl_indices.size()){
                    s.applyGateX(ctrl_indices[revIdx]);
                }
                else{
                    s.applyGateX(target);
                }
            }
        }

        /**
         * @brief Takes bitstring as the binary pattern and indices as the qubits to operate upon. Applies the appropriate PauliX gates to the control lines to call the NCU with the given matrix
         * 
         * @param s Simulator object
         * @param bitstring 
         * @param ctrlIndices 
         * @param target 
         * @return decltype(auto) 
         */
        void bitStringPhaseOracle(SimulatorType& s, std::size_t bitstring, const std::vector<std::size_t>&ctrlIndices, std::size_t target ){
            std::size_t num_qubits = s.getNumQubits();
            assert ( (1<<num_qubits) < bitstring );
            bitStringNCU(s, bitstring, ctrlIndices, target, s.getGateZ());
        }
    
    };
};
#endif
