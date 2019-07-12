/**
 * @file hamming.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Functions to compute the Hamming distance between a test pattern and the states in memory.
 * @version 0.1
 * @date 2019-14-06
 */

#ifndef QNLP_HAMMING
#define QNLP_HAMMING

//#include "Simulator.hpp"
//#include <complex>
#include <cassert>
#include <utility>
#include <memory>
#include <cmath>
#include<vector>
#include<complex>

namespace QNLP{
    template <class SimulatorType>
    class HammingDistance{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            Mat2x2Type U_ancilla;
            Mat2x2Type U_memory;

            std::size_t len_bin_pattern;

        public:
            HammingDistance(){};

            HammingDistance(const std::size_t len_bin_pattern_){
                len_bin_pattern = len_bin_pattern_;

                initialiseMats();
            };

            ~HammingDistance(){
                clearMats();    
            };

            /**
             * @brief Define the PauliX and the unitary matrix S
             * 
             */
            //template <class Type>
            void initialiseMats(){
                double pi_term, cos_term, sin_term;

                pi_term = 0.5*M_PI/(double)len_bin_pattern;
                cos_term = cos(pi_term);
                sin_term = sin(pi_term);

                // U for simulating Hamiltonian
                U_ancilla(0, 0) = std::complex<double>( cos_term,     0.0);
                U_ancilla(0, 1) = std::complex<double>( 0.0,          0.0 );
                U_ancilla(1, 0) = std::complex<double>( 0.0,          0.0 );
                U_ancilla(1, 1) = std::complex<double>( cos_term,     0.0);

                U_memory(0, 0) = std::complex<double>( cos_term,     sin_term );
                U_memory(0, 1) = std::complex<double>( 0.0,          0.0 );
                U_memory(1, 0) = std::complex<double>( 0.0,          0.0 );
                U_memory(1, 1) = std::complex<double>( cos_term,    -sin_term );
            }

            /**
             * @brief Clears the S matricess 
             * 
             */
            void clearMats(){
            }

            /**
             * @brief Encodes each element of inputted vector as a binary string in a superpostiion of states.
             * 
             * @param qSim Quantum simulator instance.
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_ancilla A vector containing the indices of the qubits of the ancilla register. 
             * @param len_bin_pattern length of binary pattern ie length of memory register.
             */
            void computeHammingDistance(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_ancilla, 
                    std::size_t len_bin_pattern){

 
                std::size_t len_reg_ancilla;
                len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_ancilla);

                qSim.applyGateH(reg_ancilla[len_reg_ancilla-2]);
                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateCX(reg_ancilla[i], reg_memory[i]);
                    qSim.applyGateX(reg_memory[i]);
                }

                qSim.applyGateU(U_ancilla,reg_ancilla[len_reg_ancilla-2]);

                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateU(U_memory, reg_memory[i]);
                }

                /************!!!!****/
                for(int i = len_bin_pattern-1; i > -1; i--){
                //    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateCX(reg_ancilla[i], reg_memory[i]);
                }
            }


    };

};
#endif


