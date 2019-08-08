/**
 * @file hamming_RotY_amplification.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Functions to compute the Hamming distance between a test pattern and the states in memory using y rotations for each similar qubit.
 * @version 0.1
 * @date 2019-02-08
 */

#ifndef QNLP_HAMMING_ROTY
#define QNLP_HAMMING_ROTY

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
    class HammingDistanceRotY{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            std::size_t len_bin_pattern;

        public:
            HammingDistanceRotY() = delete;

            HammingDistanceRotY(const std::size_t len_bin_pattern_){
                len_bin_pattern = len_bin_pattern_;
            };

            ~HammingDistanceRotY(){
            };

            /**
             * @brief Adjusts each state's ampitude proportional to the Hamming distanc ebetween the state's training pattern and the test pattern using rotations about y for each mattern qubit. 
             *
             * @param qSim Quantum simulator instance.
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_ancilla A vector containing the indices of the qubits of the ancilla register. 
             * @param len_bin_pattern length of binary pattern ie length of memory register.
             */
            void computeHammingDistance(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_ancilla, 
                    std::size_t len_bin_pattern, std::size_t num_bin_patterns){
                
                double theta = M_PI / (double) len_bin_pattern; 
                auto Ry = qSim.getGateI();

                Ry(0,0) = std::complex<double>( cos(theta/2), 0.);
                Ry(0,1) = std::complex<double>(-sin(theta/2), 0.);
                Ry(1,0) = std::complex<double>( sin(theta/2), 0.);
                Ry(1,1) = std::complex<double>( cos(theta/2), 0.);

                std::size_t len_reg_ancilla;
                len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_ancilla);

                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateNCU(Ry, std::vector<std::size_t> {reg_ancilla[i], reg_memory[i]}, reg_ancilla[len_reg_ancilla-2]);
                    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateX(reg_ancilla[i]);
                    qSim.applyGateNCU(Ry, std::vector<std::size_t> {reg_ancilla[i], reg_memory[i]}, reg_ancilla[len_reg_ancilla-2]);
                    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateX(reg_ancilla[i]);
                }
                /* 
                double theta = 2 * M_PI / (double) len_bin_pattern; 

                std::size_t len_reg_ancilla;
                len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_ancilla);

 //               qSim.applyGateH(reg_ancilla[len_reg_ancilla-2]);
                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("PreCX_X");
                #endif
                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateCX(reg_ancilla[i], reg_memory[i]);
                    qSim.applyGateX(reg_memory[i]);

                }
                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("PreCRY");
                #endif
                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateCRotY(reg_memory[i], reg_ancilla[len_reg_ancilla-2], theta);
                }
                #ifdef GATE_LOGGING
                qSim.getGateWriter().segmentMarkerOut("PREX_CX");
                #endif
                for(int i = len_bin_pattern-1; i > -1; i--){
                    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateCX(reg_ancilla[i], reg_memory[i]);
                }*/
            }


    };

};
#endif


