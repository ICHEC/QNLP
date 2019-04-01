#include "../include/qft.hpp"
#include "qureg/qureg.hpp"
#include <cmath>

//using namespace QNLP;

static void QNLP::applyQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx){
    for(std::size_t i = maxIdx; i > minIdx; i--){
        ApplyHadamard(qReg[i-1]);
        for(std::size_t j = i-1; j > minIdx; j--){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            QNLP::ApplyCPhaseGate(qReg, 2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
            //QNLP::ApplyCPhaseGate(qReg, 2.0*M_PI / pow(2, i-j + 1), j-1, i-1);
        }
    }
}

static void QNLP::applyIQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx){
    for(std::size_t i = minIdx; i < maxIdx+1; i++){
        for(std::size_t j = minIdx; j < i; j++){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            QNLP::ApplyCPhaseGate(qReg, -2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
            //QNLP::ApplyCPhaseGate(qReg, -2.0*M_PI / pow(2, i - j +1), j-1, i-1);
        }
        ApplyHadamard(qReg[i-1]);
    }
}

static void QNLP::ApplyCPhaseGate(QubitRegister<ComplexDP>& qReg, double angle, unsigned int control, unsigned int target){
    openqu::TinyMatrix<Type, 2, 2, 32> U;
    U(0, 0) = Type(1., 0.);
    U(0, 1) = Type(0., 0.);
    U(1, 0) = Type(0., 0.);
    U(1, 1) = Type(1., 0.)*exp(1i*angle);
    ApplyControlled1QubitGate(control, target, U);
}