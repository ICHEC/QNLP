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
#include <cmath>
#include <complex>
#include <vector>

namespace QNLP{
    template <class SimulatorType>
    class Diffusion{
    public:
        Diffusion() {};
        ~Diffusion() {};

        /**
         * @brief Application of the Grover diffusion operator to already marked register. Follows the Q = -A S_0 A structure as defined in https://arxiv.org/pdf/quant-ph/0005055.pdf
         * 
         * @param sim The quantum simulator object to apply the operator.
         * @param ctrlIndices Vector of qubit indices on ctrl lines (non NCU target)
         * @param target Target for application of the nCZ op
         */
        void applyOpDiffusion( SimulatorType& sim, const std::vector<std::size_t>& ctrlIndices, const std::size_t target){
            //For n-controlled not
            for(auto& ctrl : ctrlIndices){
                sim.applyGateH(ctrl);
                sim.applyGateX(ctrl);
            }
            sim.applyGateH(target);
            sim.applyGateX(target);
            
            sim.applyGateNCU(sim.getGateZ(), ctrlIndices, target, "Z");

            sim.applyGateX(target);
            sim.applyGateH(target);

            for(auto& ctrl : ctrlIndices){
                sim.applyGateX(ctrl);
                sim.applyGateH(ctrl);
            }
        }
    };
}
#endif

