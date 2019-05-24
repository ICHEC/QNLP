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
