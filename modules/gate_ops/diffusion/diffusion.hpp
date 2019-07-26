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

#ifndef QNLP_DIFFUSION
#define QNLP_DIFFUSION

#include <cstddef>
#include<cmath>
#include<complex>
//WIP
namespace QNLP{
    template <class SimulatorType>
    class Diffusion{
    public:
        Diffusion() {};
        ~Diffusion() {};

        /**
         * @brief Application of the Grover diffusion operator to already marked register. Follows the Q = -A S_0 A structure as defined in https://arxiv.org/pdf/quant-ph/0005055.pdf
         * 
         * @param qReg The quantum register to apply the operator.
         * @param minIdx The starting qubit index of the register to consider.
         * @param maxIdx The ending qubit index of the register to consider.
         */
        void applyOpDiffusion( SimulatorType& sim, const std::size_t ctrl_minIdx, const std::size_t ctrl_maxIdx, const std::size_t target){
            //For n-controlled not
            for(std::size_t i = ctrl_minIdx; i <= target; i++){
                sim.applyGateH(i);
                sim.applyGateX(i);
            }
            sim.applyGateNCU(sim.getGateZ(), ctrl_minIdx, ctrl_maxIdx, target);

            for(std::size_t i = ctrl_minIdx; i <= target; i++){
                sim.applyGateX(i);
                sim.applyGateH(i);
            }
        }
    };
}
#endif

