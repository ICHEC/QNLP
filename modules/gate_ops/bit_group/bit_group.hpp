/**
 * @file bit_group.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Implements grouping of bits to LSB side of register 
 * @version 0.1
 * @date 2019-04-02
 * @detailed This header file introduces routines for bit-wise shifting of set bits to the side of a register (ie |01010> -> |00011>).
 * Note: yes, we are cheating to reset the qubits by Hadamrding and measuring in Z-basis. Uncompute steps to follow.
 * 
 * @copyright Copyright (c) 2020
 */

#ifndef QNLP_BIT_GROUP
#define QNLP_BIT_GROUP

#include<algorithm>

namespace QNLP{
    /**
     * @brief Class definition for bit-wise grouping in register.
     * 
     * @tparam SimulatorType 
     */
    template <class SimulatorType>
    class BitGroup{
        private:
        /**
         * @brief Swap the qubits if `qreg_idx0` is set and `qreg_idx1` is not. 
         *        Uses Auxiliary qubits `qaux_idx` which are set to |10> at start and end of operations.
         * 
         * @param qSim Quantum simulator object derived from SimulatorGeneral
         * @param qreg_idx0 Index of qubit 0 in qSim qubit register
         * @param qreg_idx1 Index of qubit 1 in qSim qubit register
         * @param qaux_idx Indices of auxiliary qubits in qSim qubit register that are set to |10>
         */
        static void q_ops(SimulatorType& qSim, std::size_t qreg_idx0, std::size_t qreg_idx1, const std::vector<std::size_t>& qaux_idx){

            //Swap if C0 is set and C1 is not
            qSim.applyGateCCX(qreg_idx0, qaux_idx[0], qaux_idx[1]);
            qSim.applyGateCCX(qreg_idx1, qreg_idx0, qaux_idx[1]);
            qSim.applyGateCSwap(qaux_idx[1], qreg_idx0, qreg_idx1);

            //Reset Aux gate to |0>
            qSim.applyGateH(qaux_idx[1]);
            qSim.collapseToBasisZ(qaux_idx[1], 0);
        }


        /*        
        //@brief WIP:Swaps all qubits in register indices given by qreg_idx to their right-most positions. 
        //       Method works from opposing ends of register and subregisters iteratively until all qubits are in the correct positions.
        //
        //@param qSim Quantum simulator object derived from SimulatorGeneral
        //@param qreg_idx Indices of the qSim register to group to the LSB position.
        //@param qaux_idx Indices of auxiliary qubits in qSim register
        static void bit_swap_s2e(SimulatorType& qSim, const std::vector<std::size_t>& qreg_idx, const std::vector<std::size_t>& qaux_idx){
            assert(qaux_idx.size() >= 2);
            std::size_t num_qubits = qreg_idx.size();

            //Apply the op from opposite ends of register working towards centre, reduce size from left, then repeat until 2 qubits remain.
            
            qSim.applyGateX(qaux_idx[0]);
            for(int num_offset=0; num_offset < qreg_idx.size()/2 + 1 + qreg_idx.size()%2; num_offset++){
                for(std::size_t i=0; i < num_qubits/2 + num_qubits%2; i++){
                    if (i + num_offset == num_qubits - (i+1) + num_offset)
                        continue;
                    q_ops(qSim, qreg_idx[i + num_offset], qreg_idx[num_qubits - (i+1) + num_offset], qaux_idx);
                }
                num_qubits--;
            }
            qSim.applyGateX(qaux_idx[0]);
        }*/

        /**
         * @brief Swaps all qubits in register indices given by qreg_idx to their right-most positions. 
         *        Method works by pairing qubits using even-odd indexed cycles iteratively until all qubits are in the correct positions.
         *        
         * 
         * @param qSim Quantum simulator object derived from SimulatorGeneral
         * @param qreg_idx Indices of the qSim register to group to the LSB position.
         * @param qaux_idx Indices of auxiliary qubits in qSim register in |00> state
         */
        static void bit_swap_pair(SimulatorType& qSim, const std::vector<std::size_t>& qreg_idx, const std::vector<std::size_t>& qaux_idx, bool lsb){
            bool isOdd = qreg_idx.size() % 2;
            qSim.applyGateX(qaux_idx[0]);

            for(std::size_t num_steps = 0; num_steps < qreg_idx.size(); num_steps++){
                for(int i = 0; i < qreg_idx.size()-1; i+=2){
                    if(i + 1 + isOdd < qreg_idx.size()){
                        if(lsb)
                            q_ops(qSim,  qreg_idx[i + isOdd], qreg_idx[i + 1 + isOdd], qaux_idx);
                        else
                            q_ops(qSim,  qreg_idx[i + 1 + isOdd], qreg_idx[i + isOdd], qaux_idx);

                    }
                }
                isOdd = !isOdd;
            }
            qSim.applyGateX(qaux_idx[0]);
        }

        public:

        /**
         * @brief Swaps all qubits in register indices given by qreg_idx to their right-most positions. 
         *        Method works by pairing qubits using even-odd indexed cycles iteratively until all qubits are in the correct positions.
         *        Intermediate qubit reset operation may not be realisable on all platforms (Hadarmard followed by SigmaZ projection to |0>)
         * 
         * @param qSim Quantum simulator object derived from SimulatorGeneral
         * @param qreg_idx Indices of the qSim register to group to the LSB position.
         * @param qaux_idx Indices of auxiliary qubits in qSim register in |00> state
         * @param lsb Indicates if to shift the values to the MSB or LSB equivalent positions.
         */
        static void bit_group(SimulatorType& qSim, const std::vector<std::size_t>& qreg_idx, const std::vector<std::size_t>& qaux_idx, bool lsb=true){
            bit_swap_pair(qSim, qreg_idx, qaux_idx, lsb);
        }
    };
}

#endif