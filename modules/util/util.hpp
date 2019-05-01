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
     * @param qReg The quantum register holding the bitstrings r1,r2.
     * @param r1_min The lowest bounded r1 index to perform Fourier transform on.
     * @param r1_max The highest bounded r1 index to perform Fourier transform on.
     * @param r2_min The lowest bounded r2 index to perform Fourier transform on.
     * @param r2_max The highest bounded r2 index to perform Fourier transform on.
     */
    static void sum_reg(QubitRegister<ComplexDP>& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max);
    
    /**
     * @brief Implements |r1>|r2> -> |r1>|r1-r2>. If r1 < r2 the state will underflow, with the subtraction continuing from |11..1>. 
     * 
     * @param r1 The quantum register holding the bitstring to remain unmodified.
     * @param r1_min The lowest bounded r1 index to perform Fourier transform on.
     * @param r1_max The highest bounded r1 index to perform Fourier transform on.
     * @param r2 The quantum register holding the bitstring to accumulate r1.
     * @param r2_min The lowest bounded r2 index to perform Fourier transform on.
     * @param r2_max The highest bounded r2 index to perform Fourier transform on.
     */
    static void sub_reg(QubitRegister<ComplexDP>& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max);

    /**
     * @brief Applies the forward QFT on the given register.
     * 
     * @param qReg The qubit register.
     * @param minIdx the lower-bounded index in the register to transform.
     * @param maxIdx the upper-bounded index in the register to transform.
     */
    static void applyQFT(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx);
    /**
     * @brief Applies the inverse QFT on the given register.
     * 
     * @param qReg The qubit register.
     * @param minIdx the lower-bounded index in the register to transform.
     * @param maxIdx the upper-bounded index in the register to transform.
     */
    static void applyIQFT(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx);

    /**
     * @brief Apply the phase gate, shifting the |1> qubit phase by the given angle.
     * 
     * @param angle \exp(i\theta), angle of phase shift.
     * @param control index of the control qubit.
     * @param target index of the target qubit.
     */
    static void ApplyCPhaseGate(QubitRegister<ComplexDP>& qReg, const double angle, const unsigned int control, const unsigned int target);

    /**
     * @brief Application of the Grover diffusion operator to already marked register.
     * 
     * @param qReg The quantum register to apply the operator.
     * @param minIdx The starting qubit index of the register to consider.
     * @param maxIdx The ending qubit index of the register to consider.
     */
    static void ApplyDiffusionOp(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx);

    /**
     * @brief Swap the states between the two given qubits
     *
     * @param qReg The quantum register to perform the swap within.
     * @param q1 Qubit index 1.
     * @param q2 Qubit index 2.
     */
    static void ApplySwap(QubitRegister<ComplexDP>& qReg, const unsigned int q1, const unsigned int q2);
    
    /**
     * @brief Inverts the qubit register order from 1,2,3...n to n,n-1,...3,2,1
     * 
     * @param qReg The quantum register to apply the operation.
     * @param minIdx The starting qubit index of the register to consider.
     * @param maxIdx The ending qubit index of the register to consider.
     */
    static void InvertRegister(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx);
};
}
