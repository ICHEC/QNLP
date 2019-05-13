#include "qft.hpp"
#include "qureg/qureg.hpp"
#include <cmath>

using namespace QNLP;

void QFT::applyQFT(ISimulator& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    for(std::size_t i = maxIdx; i > minIdx; i--){
        qReg.applyGateH(i-1);
        for(std::size_t j = i-1; j > minIdx; j--){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            qReg.applyGateCPhaseShift(2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
        }
    }
}

void QFT::applyIQFT(ISimulator& qReg, const unsigned int minIdx, const unsigned int maxIdx){
    for(std::size_t i = minIdx+1; i < maxIdx+1; i++){
        for(std::size_t j = minIdx+1; j < i; j++){
            // Note:  1<<(1 + (i-j)) is 2^{i-j+1}, the respective phase term divisor
            qReg.applyGateCPhaseShift(-2.0*M_PI / (1<<(1 + (i-j))), j-1, i-1);
        }
        qReg.applyGateH(i-1);
    }
}
