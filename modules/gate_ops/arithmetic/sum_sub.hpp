/**
 * @file sum_sub.hpp.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Implements basic summation and subtraction of quantum bitstrings
 * @version 0.1
 * @date 2019-04-02
 * @detailed This header file introduces routines for bit-wise summation and subtraction of qubits. The given implementation loosely follows the approaches taken by https://github.com/hkhetawat/QArithmetic and outlined in detail by arXiv:quant-ph/0008033. While the runtime as discussed by Draper in the mentioned paper is not optimal, more complex routines require a larger number of qubits to implement.
 * 
 * @copyright Copyright (c) 2019
 */
//#include "qureg/qureg.hpp"

namespace QNLP{
    class SumSub{
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
        void sum_reg(ISimulator& rReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max);
        
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
        void sub_reg(ISimulator& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max);
    };

}
