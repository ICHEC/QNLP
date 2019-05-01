/**
 * @file utils.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief 
 * @version 0.1
 * @date 2019-04-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "util.hpp"
#include "ncu.hpp"
#include "qureg/qureg.hpp"
#include <cmath>

using namespace QNLP;

void Util::sum_reg(QubitRegister<ComplexDP>& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max){
    std::size_t num_qubits_r1 = r1_max - r1_min;
    std::size_t num_qubits_r2 = r2_max - r2_min;

    assert( num_qubits_r1 == num_qubits_r2 );

    Util::applyQFT(qReg, r2_min, r2_max);
    for(int i = num_qubits_r2; i > 0; i--){
        for(int j = i; j > 0; j--){
            ApplyCPhaseGate(qReg, 2.0*M_PI / (1<<(1 + (i-j))), r1_min + (j-1), r2_min + (i-1));
        }
    }
    Util::applyIQFT(qReg, r2_min, r2_max);
}

void Util::sub_reg(QubitRegister<ComplexDP>& qReg, const unsigned int r1_min, const unsigned int r1_max, const unsigned int r2_min, const unsigned int r2_max){
    std::size_t num_qubits_r1 = r1_max - r1_min;
    std::size_t num_qubits_r2 = r2_max - r2_min;

    //assert( num_qubits_r1 == num_qubits_r2 );

    //Flip all states in r1
    for(int i = r1_min; i < r1_max; i++){
        qReg.ApplyPauliX(i);
    }
    Util::sum_reg(qReg, r1_min, r1_max, r2_min, r2_max);
    //Flip states to return the sum
    for(int i = r2_min; i < r2_max; i++){
        qReg.ApplyPauliX(i);
    }    
    //Return all qubits in r1 to original state 
    for(int i = r1_min; i < r1_max; i++){
        qReg.ApplyPauliX(i);
    }
}

void Util::applyQFT(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    for(std::size_t ctrl = maxIdx; ctrl <= minIdx; ctrl--){
        qReg.ApplyHadamard(ctrl);
        for(std::size_t tgt = ctrl - 1; tgt <= maxIdx; tgt--){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            // QNLP::ApplyCPhaseGate(qReg, 2.0*M_PI / pow(2, i-j + 1), j-1, i-1);
            std::cout << " QFT:={" << tgt << "," << ctrl << "}"<< std::endl;
            //QNLP::Util::ApplyCPhaseGate(qReg, 2.0*M_PI / ( 1 << ( 1 + (ctrl - target) ) ), ctrl, target);
            QNLP::Util::ApplyCPhaseGate(qReg, -M_PI / (1 << (ctrl-tgt)), ctrl, tgt);
        }
    }
}

void Util::applyIQFT(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    for(std::size_t target = minIdx; target <= maxIdx; target++){
        for(std::size_t ctrl = target + 1; ctrl <= maxIdx; ctrl++){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            // QNLP::ApplyCPhaseGate(qReg, -2.0*M_PI / pow(2, i - j +1), j-1, i-1);
            std::cout << "IQFT:={" << target << "," << ctrl << "}"<< std::endl;
            QNLP::Util::ApplyCPhaseGate(qReg, -2.0*M_PI / ( 1 << ( 1 + (ctrl-target) ) ), ctrl, target);
        }
        qReg.ApplyHadamard(target);
    }
}

void Util::ApplySwap(QubitRegister<ComplexDP>& qReg, const unsigned int q1, const unsigned int q2){
    qReg.ApplyCPauliX(q1,q2); 
    qReg.ApplyCPauliX(q2,q1); 
    qReg.ApplyCPauliX(q1,q2); 
}

void Util::InvertRegister(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    unsigned int range2 = ((maxIdx - minIdx)%2 == 1) ? (maxIdx - minIdx)/2 +1 : (maxIdx - minIdx)/2;
    for(unsigned int idx = 0; idx < range2; idx++){
        std::cout << "Swapping " << minIdx+idx << "," << maxIdx-idx << std::endl;
        Util::ApplySwap(qReg, minIdx+idx, maxIdx-idx);
    }
}

void Util::ApplyCPhaseGate(QubitRegister<ComplexDP>& qReg, const double angle, const unsigned int control, const unsigned int target){
    openqu::TinyMatrix<ComplexDP, 2, 2, 32> U;
    U(0, 0) = ComplexDP(1., 0.);
    U(0, 1) = ComplexDP(0., 0.);
    U(1, 0) = ComplexDP(0., 0.);
    U(1, 1) = ComplexDP(cos(angle), sin(angle));
    qReg.ApplyControlled1QubitGate(control, target, U);
}

void Util::ApplyDiffusionOp(QubitRegister<ComplexDP>& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    //For n-controlled not
    openqu::TinyMatrix<ComplexDP, 2, 2, 32> X;
    X(0,0) = {0.,  0.};
    X(0,1) = {1., 0.};
    X(1,0) = {1., 0.};
    X(1,1) = {0.,  0.};

    for(unsigned int i = minIdx; i <= maxIdx; i++){
        qReg.ApplyHadamard(i);
    }
    for(unsigned int i = minIdx; i <= maxIdx; i++){
        qReg.ApplyPauliX(i);
    }
    qReg.ApplyHadamard(maxIdx);

    NQubitDecompose<ComplexDP> nCtrlX(X, (maxIdx-minIdx)-1);
    nCtrlX.applyNQubitControl(qReg, minIdx, maxIdx-1, maxIdx, X, 0, true);

    qReg.ApplyHadamard(maxIdx);
    for(unsigned int i = minIdx; i <= maxIdx; i++){
        qReg.ApplyPauliX(i);
    }
    for(unsigned int i = minIdx; i <= maxIdx; i++){
        qReg.ApplyHadamard(i);
    }
}
