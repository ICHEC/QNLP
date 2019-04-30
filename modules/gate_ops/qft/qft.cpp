#include "qft.hpp"
#include "qureg/qureg.hpp"
#include <cmath>

//using namespace QNLP;

void QNLP::Ops::applyQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx){
    for(std::size_t i = maxIdx; i > minIdx; i--){
        qReg.ApplyHadamard(i-1);
        for(std::size_t j = i-1; j > minIdx; j--){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            // QNLP::ApplyCPhaseGate(qReg, 2.0*M_PI / pow(2, i-j + 1), j-1, i-1);
            QNLP::Ops::ApplyCPhaseGate(qReg, 2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
        }
    }
}

void QNLP::Ops::applyIQFT(QubitRegister<ComplexDP>& qReg, unsigned int minIdx, unsigned int maxIdx){
    for(std::size_t i = minIdx+1; i < maxIdx+1; i++){
        for(std::size_t j = minIdx+1; j < i; j++){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            // QNLP::ApplyCPhaseGate(qReg, -2.0*M_PI / pow(2, i - j +1), j-1, i-1);
            QNLP::Ops::ApplyCPhaseGate(qReg, -2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
        }
        qReg.ApplyHadamard(i-1);
    }
}

void QNLP::Ops::ApplyCPhaseGate(QubitRegister<ComplexDP>& qReg, double angle, unsigned int control, unsigned int target){
    openqu::TinyMatrix<ComplexDP, 2, 2, 32> U;
    U(0, 0) = ComplexDP(1., 0.);
    U(0, 1) = ComplexDP(0., 0.);
    U(1, 0) = ComplexDP(0., 0.);
    U(1, 1) = ComplexDP(cos(angle), sin(angle));
    qReg.ApplyControlled1QubitGate(control, target, U);
}
