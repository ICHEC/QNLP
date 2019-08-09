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
            s.applyGateNCU(U, ctrl_indices, target);
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
         * @param s 
         * @param bitstring 
         * @param ctrl_indices 
         * @param U Unitary matrix to apply 
         */
        void bitStringPhaseOracle(SimulatorType& s, std::size_t bitstring, const std::vector<std::size_t>& ctrl_indices, std::size_t target){
            std::size_t num_qubits = s.getNumQubits();
            assert ( (1<<num_qubits) < bitstring );
            bitStringNCU(s, bitstring, ctrl_indices, target, s.getGateZ());
        }
    };
};
#endif
