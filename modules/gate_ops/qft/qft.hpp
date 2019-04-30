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
            static void applyQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx);
            /**
             * @brief Applies the inverse QFT on the given register
             * 
             * @param qReg The qubit register
             * @param minIdx the lower-bounded index in the register to transform
             * @param maxIdx the upper-bounded index in the register to transform
             */
            static void applyIQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx);

            /**
             * @brief Apply the phase gate, shifting the |1> qubit phase by the given angle.
             * 
             * @param angle \exp(i\theta), angle of phase shift
             * @param control index of the control qubit
             * @param target index of the target qubit
             */
            static void ApplyCPhaseGate(QubitRegister<ComplexDP>& qReg, double angle, unsigned int control, unsigned int target);
    };

}
