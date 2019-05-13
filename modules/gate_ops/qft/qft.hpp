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
#include "qureg/qureg.hpp"
#include "Simulator.hpp"

namespace QNLP{
    class QFT{
        public:
            /**
             * @brief Applies the forward QFT on the given register
             * 
             * @param qReg The qubit register
             * @param minIdx the lower-bounded index in the register to transform
             * @param maxIdx the upper-bounded index in the register to transform
             */
            static void applyQFT(ISimulator& qReg, const unsigned int minIdx, const unsigned int maxIdx);

            /**
             * @brief Applies the inverse QFT on the given register
             * 
             * @param qReg The qubit register
             * @param minIdx the lower-bounded index in the register to transform
             * @param maxIdx the upper-bounded index in the register to transform
             */
            static void applyIQFT(ISimulator& qReg, const unsigned int minIdx, const unsigned int maxIdx);
    };

}
