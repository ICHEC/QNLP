/**
 * @file sum_sub.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Implements basic summation and subtraction of quantum bitstrings
 * @version 0.1
 * @date 2019-04-02
 * @detailed This header file introduces routines for bit-wise summation and subtraction of qubits. The given implementation loosely follows the approaches taken by https://github.com/hkhetawat/QArithmetic and outlined in detail by arXiv:quant-ph/0008033. While the runtime as discussed by Draper in the mentioned paper is not optimal, more complex routines require a larger number of qubits to implement.
 * 
 * @copyright Copyright (c) 2019
 */

#ifndef QNLP_ARITHMETIC
#define QNLP_ARITHMETIC

namespace QNLP{
    template <class SimulatorType>
    class Arithmetic{
        public:
        using CST = const std::size_t;
        /**
         * @brief Implements |r1>|r2> -> |r1>|r1+r2>. Required bits to represent |r1+r2> should be available prior to calling, or overflow will occur.
         * 
         * @param qReg The quantum register holding the bitstrings r1,r2
         * @param r1_min The lowest bounded r1 index to perform Fourier transform on.
         * @param r1_max The highest bounded r1 index to perform Fourier transform on.
         * @param r2_min The lowest bounded r2 index to perform Fourier transform on.
         * @param r2_max The highest bounded r2 index to perform Fourier transform on.
         */
        void sum_reg(SimulatorType& qSim, CST r1_min, CST r1_max, CST r2_min, CST r2_max){
            std::size_t num_qubits_r1 = r1_max - r1_min;
            std::size_t num_qubits_r2 = r2_max - r2_min;

            assert( num_qubits_r1 == num_qubits_r2 );

            qSim.applyQFT(r2_min, r2_max);
            for(int i = num_qubits_r2; i > 0; i--){
                for(int j = i; j > 0; j--){
                    qSim.applyGateCPhaseShift(2.0*M_PI / (1<<(1 + (i-j))), r1_min + (j-1), r2_min + (i-1));
                }
            }
            qSim.applyIQFT(r2_min, r2_max);
        }
        
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
        void sub_reg(SimulatorType& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max){
            std::size_t num_qubits_r1 = r1_max - r1_min;
            std::size_t num_qubits_r2 = r2_max - r2_min;

            assert( num_qubits_r1 == num_qubits_r2 );

            //Flip all states in r1
            for(int i = r1_min; i < r1_max; i++){
                qReg.applyGateX(i);
            }
            sum_reg(qReg, r1_min, r1_max, r2_min, r2_max);
            //Flip states to return the sum
            for(int i = r2_min; i < r2_max; i++){
                qReg.applyGateX(i);
            }    
            //Return all qubits in r1 to original state 
            for(int i = r1_min; i < r1_max; i++){
                qReg.applyGateX(i);
            }
        }
    };
}

#endif