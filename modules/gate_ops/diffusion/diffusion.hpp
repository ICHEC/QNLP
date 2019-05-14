/**
 * @file diffusion.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief 
 * @version 0.1
 * @date 2019-04-02
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "ncu.hpp"

namespace QNLP{
    template <class SimulatorType>
    class Diffusion{
    public:

        /**
         * @brief Application of the Grover diffusion operator to already marked register.
         * 
         * @param qReg The quantum register to apply the operator.
         * @param minIdx The starting qubit index of the register to consider.
         * @param maxIdx The ending qubit index of the register to consider.
         */
        void applyOpDiffusion(SimulatorType& sim, const unsigned int minIdx, const unsigned int maxIdx){
            //For n-controlled not
            for(unsigned int i = minIdx; i <= maxIdx; i++){
                sim.applyGateH(i);
            }
            for(unsigned int i = minIdx; i <= maxIdx; i++){
                sim.applyGateX(i);
            }
            sim.applyGateH(maxIdx);

            NCU<decltype(sim.getGateX()), decltype(sim)> nCtrlX(sim.getGateX(), (maxIdx-minIdx)-1);
            NCU.applyNQubitControl(sim, minIdx, maxIdx-1, maxIdx, s.getGateX(), 0, true);

            sim.applyGateH(maxIdx);
            for(unsigned int i = minIdx; i <= maxIdx; i++){
                sim.applyGateX(i);
            }
            for(unsigned int i = minIdx; i <= maxIdx; i++){
                sim.applyGateH(i);
            }
        }
    };
}
