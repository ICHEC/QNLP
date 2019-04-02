/**
 * @file utils.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief 
 * @version 0.1
 * @date 2019-04-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "qureg/qureg.hpp"

namespace QNLP{
class Util{
    public:
    /**
     * @brief Implements |r1>|r2> -> |r1>|r1+r2>. Required bits to represent |r1+r2> should be available prior to calling, or overflow will occur.
     * 
     * @param qReg The quantum register holding the bitstrings r1,r2
     * @param r1_min The lowest bounded r1 index to perform Fourier transform on.
     * @param r1_max The highest bounded r1 index to perform Fourier transform on.
     * @param r2_min The lowest bounded r2 index to perform Fourier transform on.
     * @param r2_max The highest bounded r2 index to perform Fourier transform on.
     */
    static void sum_reg(QubitRegister<ComplexDP>& rReg, unsigned int r1_min, unsigned int r1_max, unsigned int r2_min, unsigned int r2_max);
    
    /**
     * @brief Implements |r1>|r2> -> |r1>|r1-r2>. If r1 < r2 the state will underflow, with the subtraction continuing from |11..1>. 
     * 
     * @param r1 The quantum register holding the bitstring to remain unmodified
     * @param r1_min The lowest bounded r1 index to perform Fourier transform on.
     * @param r1_max The highest bounded r1 index to perform Fourier transform on.
     * @param r2 The quantum register holding the bitstring to accumulate r1
     * @param r2_min The lowest bounded r2 index to perform Fourier transform on.
     * @param r2_max The highest bounded r2 index to perform Fourier transform on.
     */
    static void sub_reg(QubitRegister<ComplexDP>& qReg, unsigned int r1_min, unsigned int r1_max, unsigned int r2_min, unsigned int r2_max);

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