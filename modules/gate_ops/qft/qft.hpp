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
            double theta=0;

            std::size_t value_range = maxIdx - minIdx;

            //target lines
            for(int i = minIdx; i <= maxIdx; i++){
                qSim.applyGateH(value_range - i);

                //Control lines:
                for(int j = i+1; j <= maxIdx; j++){
                    theta = 2.0*M_PI / (std::size_t) (1<<(1+(j-i)));
                    qSim.applyGateCPhaseShift(theta, value_range - j,  value_range - i);
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
            double theta=0;

            //Target lines
            for(int i = minIdx; i <= maxIdx; i++){

                //Control lines:
                for(int j = minIdx; j < i; j++){
                    theta = -2.0*M_PI / (std::size_t) (1<<(1+(i-j)));
                    qSim.applyGateCPhaseShift(theta, j,  i);
                }
                qSim.applyGateH( i );
            }
        }
    };
}
#endif
