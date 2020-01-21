/**
 * @file hamming_RotY_amplification.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Functions to compute the Hamming distance between a test pattern and the states in memory using y rotations for each similar qubit.
 * @version 0.1
 * @date 2019-02-08
 */

#ifndef QNLP_HAMMING_ROTY
#define QNLP_HAMMING_ROTY

#include <cassert>
#include <utility>
#include <memory>
#include <cmath>
#include<vector>
#include<complex>

namespace QNLP{
    /**
     * @brief Class definition for implementing the Hamming distance routine along with controlled Y rotations to encode the Hamming distance into the states' amplitudes. 
     * 
     * @tparam SimulatorType Class simulator type 
     */
    template <class SimulatorType>
    class HammingDistanceRotY{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            std::size_t len_bin_pattern;

        public:
            /**
             * @brief Construct a new Hamming Distance Rot Y object (disabled)
             * 
             */
            HammingDistanceRotY() = delete;

            /**
             * @brief Construct a new Hamming Distance Rot Y object
             * 
             * @param len_bin_pattern_ Length of binary string which the Hamming distance is to be computed upon 
             */
            HammingDistanceRotY(const std::size_t len_bin_pattern_){
                len_bin_pattern = len_bin_pattern_;
            };

            /**
             * @brief Destroy the Hamming Distance Rot Y object
             * 
             */
            ~HammingDistanceRotY(){
            };

            /**
             * @brief Computes Hamming Distance; adjusts each state's amplitude proportional to the Hamming distance between the state's training pattern and the test pattern using rotations about y for each mattern qubit. 
             *
             * @param qSim Quantum simulator instance.
             * @param reg_memory A vector containing the indices of the qubits of the memory register. 
             * @param reg_auxiliary A vector containing the indices of the qubits of the auxiliary register. 
             * @param len_bin_pattern length of binary pattern ie length of memory register.
             */
            void computeHammingDistanceRotY(SimulatorType& qSim, 
                    const std::vector<std::size_t>& reg_memory,
                    const std::vector<std::size_t>& reg_auxiliary, 
                    std::size_t len_bin_pattern){
                
                double theta = M_PI / (double) len_bin_pattern; 
                auto Ry = qSim.getGateI();

                Ry(0,0) = std::complex<double>( cos(theta/2), 0.);
                Ry(0,1) = std::complex<double>(-sin(theta/2), 0.);
                Ry(1,0) = std::complex<double>( sin(theta/2), 0.);
                Ry(1,1) = std::complex<double>( cos(theta/2), 0.);

                qSim.addUToCache("RY", Ry);

                std::size_t len_reg_auxiliary;
                len_reg_auxiliary = reg_auxiliary.size();

                // Require length of auxiliary register to have n+2 qubits
                assert(reg_memory.size() + 1 < len_reg_auxiliary);

                for(std::size_t i = 0; i < len_bin_pattern; i++){
                    qSim.applyGateNCU(Ry, std::vector<std::size_t> {reg_auxiliary[i], reg_memory[i]}, reg_auxiliary[len_reg_auxiliary-2], "RY");
                    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateX(reg_auxiliary[i]);
                    qSim.applyGateNCU(Ry, std::vector<std::size_t> {reg_auxiliary[i], reg_memory[i]}, reg_auxiliary[len_reg_auxiliary-2], "RY");
                    qSim.applyGateX(reg_memory[i]);
                    qSim.applyGateX(reg_auxiliary[i]);
                }
            }
    };

};
#endif