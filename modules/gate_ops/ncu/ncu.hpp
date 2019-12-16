/**
 * @file ncu.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Functions for applying n-qubit controlled U (unitary) gates
 * @version 0.1
 * @date 2019-03-06
 */

#ifndef QNLP_NCU
#define QNLP_NCU

#include <unordered_map>
#include <complex>
#include <cassert>
#include <utility>
#include <vector>
#include <iostream>

#include "GateCache.hpp"
#include "mat_ops.hpp"

namespace QNLP{
    template <class SimulatorType>
    class NCU{
        private:
        std::unordered_set<std::string> default_gates {"X", "Y", "Z", "I", "H"};
        /**
         * @brief For the 5+ controlled NCX decomposition routines defined within
         * https://arxiv.org/pdf/quant-ph/9503016.pdf
         * 
         * n:= number of control lines, n>=5
         * m:= multi-control gate partition 1, m \\in {2,...,n-3}
         * l:= multi-control gate partition 2, l = n-m-1
         */
        struct OptParamsCX {
            std::size_t n;
            std::size_t m;
            std::size_t l;
            std::size_t num_ops;
        };

        //Note: currently these are used for optimising CX calls only. Static would be better for perf. Optimise later
        //static std::unordered_map<std::size_t, std::size_t> op_call_counts_CX ;
        //static std::unordered_map<std::size_t, OptParamsCX> opt_op_call_params_CX ;        
        std::unordered_map<std::size_t, std::size_t> op_call_counts_CX ;
        std::unordered_map<std::size_t, OptParamsCX> opt_op_call_params_CX ;

        //Goal: replace above hashmaps, as well as sqrtMAtrices etc with the GateCache object, which holds them all.
        GateCache<SimulatorType> gate_cache;

        protected:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            static std::size_t num_gate_ops;

        public:
            NCU() {
                //Optimised building block routines for nCX
                if(op_call_counts_CX.size() == 0){
                    op_call_counts_CX[3] = 13;
                    op_call_counts_CX[5] = 60;
                }
            };

            NCU(SimulatorType& qSim) : NCU(){
                gate_cache = GateCache<SimulatorType>(qSim);
            }

            ~NCU(){
                clearMaps();
                gate_cache.clearCache();
            };

            /**
             * @brief Add the PauliX and the given unitary U to the maps
             * 
             * @param U 
             */
            void initialiseMaps( SimulatorType& qSim,  std::size_t num_ctrl_lines){
                gate_cache.initCache(qSim, num_ctrl_lines);
                /*for( auto& [k,v] : gate_cache.gateCacheMap ){
                    std::cout << "KEYS=" << k << std::endl;
                    std::cout << "VALS=" << v[2].first.tostr() << "/:/" << v[2].second.tostr() << std::endl;
                }*/
            }

            /**
             * @brief Add the given unitary matrix to the maps up to the required depth
             * 
             * @param U 
             */
            void addToMaps( SimulatorType& qSim, std::string U_label, const Mat2x2Type& U, std::size_t num_ctrl_lines){
                gate_cache.addToCache(qSim, U_label, U, num_ctrl_lines);
            }

            /**
             * @brief Get the Map of cached gates. Keys are strings, and values are vectors of paired (gate, gate adjoint) types where the index give the value of (gate)^(1/2^i)
             * 
             * @return GateCache<SimulatorType> type 
             */
            GateCache<SimulatorType>& getGateCache(){
                return gate_cache;
            }

            /**
             * @brief Clears the maps of stored sqrt matrices
             * 
             */
            void clearMaps(){
                gate_cache.clearCache();
            }


            /**
             * @brief Decompose n-qubit controlled op into 1 and 2 qubit gates. Control indices can be in any specified location. Ensure the gate cache has been populated with the appropriate gate type before running. This avoids O(n) checking of the container at each call for the associated gates.
             * 
             * @tparam Type ComplexDP or ComplexSP 
             * @param qReg Qubit register
             * @param ctrlIndices Vector of indices for control lines
             * @param qTarget Target qubit for the unitary matrix U
             * @param U Unitary matrix, U
             * @param depth Depth of recursion.
             */
            void applyNQubitControl(SimulatorType& qSim, 
                    const std::vector<std::size_t> ctrlIndices,
                    const std::vector<std::size_t> auxIndices,
                    const unsigned int qTarget,
                    const std::string gateLabel,
                    const Mat2x2Type& U,
                    const std::size_t depth
            ){
                //No safety checks; be aware of what is physically possible (qTarget not in control_indices)
                int local_depth = depth + 1;

                //Determine the range over which the qubits exist; consider as a count of the control ops, hence +1 since extremeties included
                std::size_t cOps = ctrlIndices.size();
                //std::cout << auxIndices.size() << std::endl;
//LAST HURDLE
/*                if( (cOps >= 6) && (auxIndices.size() >= 1) && (gateLabel == "X") && (depth == 0) ){
                    auto params = find_optimal_params( ctrlIndices.size() );

                    //Need at least 1 aux qubit to perform decomposition
                    assert( auxIndices.size() > 0);
                    
                    //Gate step 1 setup
                    assert( ctrlIndices.size() >= params.m );
                    std::vector<std::size_t> subMCtrlIndicesNCX(ctrlIndices.begin(), ctrlIndices.begin() + params.m);
                    std::vector<std::size_t> subMAuxIndicesNCX(ctrlIndices.begin() + params.m,ctrlIndices.end());
                    subMAuxIndicesNCX.push_back(qTarget);

                    //Gate step 2 setup
                    std::vector<std::size_t> subLCtrlIndicesNCX(ctrlIndices.begin() + params.m, ctrlIndices.end());
                    subLCtrlIndicesNCX.push_back(auxIndices[0]);
                    std::vector<std::size_t> subLAuxIndicesNCX(ctrlIndices.begin(), ctrlIndices.begin() + params.m);

                    applyNQubitControl(qSim, subMCtrlIndicesNCX, subMAuxIndicesNCX, auxIndices[0], "X", qSim.getGateX(), 0 );
                    applyNQubitControl(qSim, subLCtrlIndicesNCX, subLAuxIndicesNCX, qTarget, "X", qSim.getGateX(), 0 );
                    applyNQubitControl(qSim, subMCtrlIndicesNCX, subMAuxIndicesNCX, auxIndices[0], "X", qSim.getGateX(), 0 );
                    applyNQubitControl(qSim, subLCtrlIndicesNCX, subLAuxIndicesNCX, qTarget, "X", qSim.getGateX(), 0 );

                }
*/
                /*if( (cOps == 5) && (auxIndices.size() >= 3) && (gateLabel == "X") && (depth == 0) ){ //161 -> 60 2-qubit gate calls

                    qSim.applyGateCCX(ctrlIndices[4], auxIndices[2], qTarget);
                    qSim.applyGateCCX(ctrlIndices[3], auxIndices[1], auxIndices[2]);
                    qSim.applyGateCCX(ctrlIndices[2], auxIndices[0], auxIndices[1]);
                    qSim.applyGateCCX(ctrlIndices[0], ctrlIndices[1], auxIndices[0]);
                    qSim.applyGateCCX(ctrlIndices[2], auxIndices[0], auxIndices[1]);
                    qSim.applyGateCCX(ctrlIndices[3], auxIndices[1], auxIndices[2]);
                    qSim.applyGateCCX(ctrlIndices[4], auxIndices[2], qTarget);

                    qSim.applyGateCCX(ctrlIndices[3], auxIndices[1], auxIndices[2]);
                    qSim.applyGateCCX(ctrlIndices[2], auxIndices[0], auxIndices[1]);
                    qSim.applyGateCCX(ctrlIndices[0], ctrlIndices[1], auxIndices[0]);
                    qSim.applyGateCCX(ctrlIndices[2], auxIndices[0], auxIndices[1]);
                    qSim.applyGateCCX(ctrlIndices[3], auxIndices[1], auxIndices[2]);
                }
*/
                if( (cOps >= 5) && ( auxIndices.size() >= cOps-2 ) && (gateLabel == "X") && (depth == 0) ){ //161 -> 60 2-qubit gate calls
                    //std::cout << "I AM HERE0! " << cOps << "  " << auxIndices.size() << "    " << ctrlIndices.size() << std::endl;
                    qSim.applyGateCCX( ctrlIndices.back(), auxIndices.back(), qTarget);
                    for (std::size_t i = ctrlIndices.size()-2; i >= 2; i--){
                        qSim.applyGateCCX( *(ctrlIndices.begin()+i), *(auxIndices.begin() + (i-2)), *(auxIndices.begin() + (i-1)));
                    }
                    qSim.applyGateCCX( *(ctrlIndices.begin()), *(ctrlIndices.begin()+1), *(auxIndices.begin()) );
                    
                    for (std::size_t i = 2; i < ctrlIndices.size(); i++){
                        qSim.applyGateCCX( *(ctrlIndices.begin()+i), *(auxIndices.begin()+(i-2)), *(auxIndices.begin()+(i-1)));
                    }
                    qSim.applyGateCCX( ctrlIndices.back(), auxIndices.back(), qTarget);

                    for (std::size_t i = ctrlIndices.size()-2; i >= 2; i--){
                        qSim.applyGateCCX( *(ctrlIndices.begin()+i), *(auxIndices.begin() + (i-2)), *(auxIndices.begin() + (i-1)));
                    }
                    qSim.applyGateCCX( *(ctrlIndices.begin()), *(ctrlIndices.begin()+1), *(auxIndices.begin()) );    
                    for (std::size_t i = 2; i < ctrlIndices.size(); i++){
                        qSim.applyGateCCX( *(ctrlIndices.begin()+i), *(auxIndices.begin()+(i-2)), *(auxIndices.begin()+(i-1)));
                    }
                }

                else if(cOps == 3){ //Optimisation for replacing 17 with 13 2-qubit gate calls
                    //std::cout << "I AM HERE1! " << cOps << "  " << auxIndices.size() << "    " << ctrlIndices.size() << std::endl;

                    //Apply the 13 2-qubit gate calls
                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].first, ctrlIndices[0], qTarget, gateLabel );
                    //std::cout << "G1[" << ctrlIndices[0] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].first.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[1]);
                    //std::cout << "G2[" << ctrlIndices[0] << "," << ctrlIndices[1] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].second, ctrlIndices[1], qTarget, gateLabel );
                    //std::cout << "G3[" << ctrlIndices[1] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].second.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[1]);
                    //std::cout << "G4[" << ctrlIndices[0] << "," << ctrlIndices[1] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].first, ctrlIndices[1], qTarget, gateLabel );
                    //std::cout << "G5[" << ctrlIndices[1] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].first.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[1], ctrlIndices[2]);
                    //std::cout << "G6[" << ctrlIndices[1] << "," << ctrlIndices[2] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].second, ctrlIndices[2], qTarget, gateLabel );
                    //std::cout << "G7[" << ctrlIndices[2] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].second.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[2]);
                    //std::cout << "G8[" << ctrlIndices[0] << "," << ctrlIndices[2] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].first, ctrlIndices[2], qTarget, gateLabel );
                    //std::cout << "G9[" << ctrlIndices[2] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].first.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[1], ctrlIndices[2]);
                    //std::cout << "G10[" << ctrlIndices[1] << "," << ctrlIndices[2] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].second, ctrlIndices[2], qTarget, gateLabel );
                    //std::cout << "G11[" << ctrlIndices[2] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].second.tostr() << std::endl;

                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[2]);
                    //std::cout << "G12[" << ctrlIndices[0] << "," << ctrlIndices[2] << "]=" << qSim.getGateX().tostr() << std::endl;

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth+1].first, ctrlIndices[2], qTarget, gateLabel );
                    //std::cout << "G13[" << ctrlIndices[2] << "," << qTarget << "]=" << gate_cache.gateCacheMap[gateLabel][local_depth+1].first.tostr() << std::endl;
                }

                else if (cOps >= 2 && cOps !=3){
                    //std::cout << "I AM HERE2! " << cOps << "  " << auxIndices.size() << "    " << ctrlIndices.size() << std::endl;

                //if (cOps >= 2){
                    std::vector<std::size_t> subCtrlIndices(ctrlIndices.begin(), ctrlIndices.end()-1);

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth].first, ctrlIndices.back(), qTarget, gateLabel );
                    //std::cout << "Here 1 = " << local_depth << " GATE = " << gate_cache.gateCacheMap[gateLabel][local_depth].first.tostr() << std::endl;

                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, ctrlIndices.back(), "X", qSim.getGateX(), 0 );

                    qSim.applyGateCU( gate_cache.gateCacheMap[gateLabel][local_depth].second, ctrlIndices.back(), qTarget, gateLabel );
                    //std::cout << "Here 2 = " << local_depth << " GATE = " << gate_cache.gateCacheMap[gateLabel][local_depth].second.tostr() << std::endl;

                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, ctrlIndices.back(), "X", qSim.getGateX(), 0 );

                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, qTarget, gateLabel, gate_cache.gateCacheMap[gateLabel][local_depth+1].first, local_depth );
                }

                //If the number of control qubits is less than 2, assume we have decomposed sufficiently
                else{
                    //std::cout << "I AM HERE3! " << cOps << "  " << auxIndices.size() << "    " << ctrlIndices.size() << std::endl;

                    qSim.applyGateCU(gate_cache.gateCacheMap[gateLabel][depth].first, ctrlIndices[0], qTarget, gateLabel); //The first decomposed matrix value is used here
                    //std::cout << "Here 4 = " << local_depth << " GATE = " << gate_cache.gateCacheMap[gateLabel][depth].first.tostr() << std::endl;
                }
            }

        /**
         * @brief Returns the number of 2-qubit operations a non optimised 
         *          decomposition will make. Cache intermediate results
         * 
         * @param num_ctrl_lines The number of control lines in the NCU call
         * @return std::size_t The number of 2-qubit gate calls
         */
        std::size_t get_op_calls(std::size_t num_ctrl_lines){
            std::size_t num_ops = 0;
            auto it = op_call_counts_CX.find(num_ctrl_lines);
            if ( it != op_call_counts_CX.end() ){
                std::cout << "get_op_calls CTRL=" << num_ctrl_lines << "  OPS=" << it->second << std::endl;

                return it->second;
            }
            else if (num_ctrl_lines >= 2){
                num_ops += (2 + 3*get_op_calls(num_ctrl_lines-1));
            }
            else{
                num_ops += 1;
            }
            op_call_counts_CX[num_ctrl_lines] = num_ops;
            std::cout << "get_op_calls CTRL=" << num_ctrl_lines << "  OPS=" << num_ops << std::endl;

            return num_ops;
        }

        /**
         * @brief Helper method to get optimised number of 2-gate ops 
         * for given decomposition params
         * 
         * @param l multi-control gate partition 2, l = n-m-1
         * @param m multi-control gate partition 1, m \\in {2,...,n-3}
         * @return const std::size_t 
         */
        inline const std::size_t get_ops_for_params(std::size_t l, std::size_t m){
            return 2*(get_op_calls(m) + get_op_calls(l));
        }

        /**
         * @brief Returns the number of 2-qubit operations an optimised 
         *          decomposition will make for nCX. Caches intermediate results.
         * 
         * @param num_ctrl_lines The number of control lines in the nCX call
         * @return std::size_t The number of 2-qubit gate calls
         */
        OptParamsCX find_optimal_params(std::size_t num_ctrl_lines){

            //If entry exists, return it
            auto it = opt_op_call_params_CX.find(num_ctrl_lines);
            if ( it != opt_op_call_params_CX.end() ){
                std::cout << "FOUND OPTIMAL PARAMS SEARCH FOR CTRL=" << num_ctrl_lines << "  M=" << it->second.m << " L=" << it->second.l << " OPS=" << it->second.num_ops << std::endl;

                return it->second;
            }
            
            //Determine number of comparisons to make.
            std::size_t num_comp = static_cast<std::size_t>(std::floor((float)(num_ctrl_lines)/2.0)) +1;

            OptParamsCX optimal_params;
            std::size_t tmp_num_ops;
            
            for(std::size_t _m = 2; _m <= num_comp; ++_m){
                std::size_t _l = (num_ctrl_lines - _m + 1);
                tmp_num_ops = get_ops_for_params(_l, _m);
                if (_m == 2){
                    optimal_params = OptParamsCX{num_ctrl_lines, _m, _l, tmp_num_ops};
                }
                else if ( tmp_num_ops < optimal_params.num_ops){
                    optimal_params = OptParamsCX{num_ctrl_lines, _m, _l, tmp_num_ops};
                }
                std::cout << "OPTIMAL PARAMS SEARCH FOR CTRL=" << num_ctrl_lines << "  M=" << _m << " L=" << _l << " OPS=" << tmp_num_ops << std::endl;
            }
            opt_op_call_params_CX[num_ctrl_lines] = optimal_params;
            op_call_counts_CX[num_ctrl_lines] = optimal_params.num_ops;
            std::cout << "OPTIMAL PARAMS FOR CTRL=" << num_ctrl_lines << "  M=" << optimal_params.m << " L=" << optimal_params.l << " OPS=" << optimal_params.num_ops << std::endl;
            return optimal_params;
        }
    };

};
#endif
