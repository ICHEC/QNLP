/**
 * @file qft.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Implements the forward and inverse quantum Fourier transform
 * @version 0.1
 * @date 2019-04-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef QNLP_QFT
#define QNLP_QFT

#include <cmath>

namespace QNLP{
    template <class SimulatorType>
    class QFT{
        public:
        /**
         * @brief Applies the forward QFT on the given register
         * 
         * @param qSim The qubit register
         * @param minIdx the lower-bounded index in the register to transform
         * @param maxIdx the upper-bounded index in the register to transform
         */
        static void applyQFT(SimulatorType& qSim, const unsigned int minIdx, const unsigned int maxIdx){
            for(std::size_t i = maxIdx; i > minIdx; i--){
                qSim.applyGateH(i-1);
                for(std::size_t j = i-1; j > minIdx; j--){
                // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
                    qSim.applyGateCPhaseShift(2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
                }
            }
        }

        /**
         * @brief Applies the inverse QFT on the given register
         * 
         * @param qSim The qubit register
         * @param minIdx the lower-bounded index in the register to transform
         * @param maxIdx the upper-bounded index in the register to transform
         */
        static void applyIQFT(SimulatorType& qSim, const unsigned int minIdx, const unsigned int maxIdx){
            for(std::size_t i = minIdx+1; i < maxIdx+1; i++){
                for(std::size_t j = minIdx+1; j < i; j++){
                    // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
                    qSim.applyGateCPhaseShift(-2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
                }
                qSim.applyGateH(i-1);
            }
        }
    };
}
#endif
