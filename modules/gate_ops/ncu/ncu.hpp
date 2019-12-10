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
                gate_cache = GateCache<SimulatorType>();
                
                //Optimised building block routines for nCX
                if(op_call_counts_CX.size() == 0){
                    op_call_counts_CX[3] = 13;
                    op_call_counts_CX[5] = 60;
                }
            };
            NCU(SimulatorType& qSim, std::string matrixLabel, const Mat2x2Type& U, const std::size_t num_ctrl_gates = 32) : NCU() {
                initialiseMaps(qSim, matrixLabel, U, num_ctrl_gates);
                gate_cache.initCache(qSim);
            };

            ~NCU(){
                clearMaps();
                gate_cache.clearCache();
            };

            /**
             * @brief Add the PauliX and the given unitary U to the maps
             * 
             * @param U 
             */
            void initialiseMaps( SimulatorType& qSim, std::string U_label, const Mat2x2Type& U, std::size_t num_ctrl_lines){
                
                Mat2x2Type phase_test = qSim.getGateZ();
                gate_cache.gateCacheMap[GateMetaData(U_label, 0, false)] = U;

                const bool is_gateZ = (qSim.getGateZ() == U);

                for(std::size_t ncl = 1; ncl < num_ctrl_lines; ncl++){
                    gate_cache.gateCacheMap[GateMetaData("X", ncl, false)] = matrixSqrt(qSim.getGateX());
                    gate_cache.gateCacheMap[GateMetaData("X", ncl, true)] = adjointMatrix(gate_cache.gateCacheMap[GateMetaData("X", ncl, false)]);

                    //If the input matrix U is a Pauli Z, use phase rotation to define sqrt gates
                    if( is_gateZ ){
                        phase_test(1,1) = exp( std::complex<double>(0,1.) * (M_PI/(0b1<<ncl)));
                        gate_cache.gateCacheMap[GateMetaData("Z", ncl, false)] = phase_test;
                        gate_cache.gateCacheMap[GateMetaData("Z", ncl, true)] = adjointMatrix(phase_test);
                    }
                    else{
                        gate_cache.gateCacheMap[GateMetaData(U_label, ncl, false)] = matrixSqrt(gate_cache.gateCacheMap[GateMetaData(U_label, ncl-1, false)]);
                        gate_cache.gateCacheMap[GateMetaData(U_label, ncl, true)] = adjointMatrix(gate_cache.gateCacheMap[GateMetaData(U_label, ncl, false)]);
                    }
                }
            }

            /**
             * @brief Add the given unitary matrix to the maps up to the required depth
             * 
             * @param U 
             */
            void addToMaps( std::string U_label, const Mat2x2Type& U, std::size_t num_ctrl_lines){
                auto gmd = GateMetaData(U_label, 0, false);
                auto it_find = gate_cache.gateCacheMap.find(gmd);

                if( it_find != gate_cache.gateCacheMap.end() ){
                    return;
                }
                gate_cache.gateCacheMap[gmd] = U;
                gate_cache.gateSet.insert(U_label);

                for(std::size_t ncl = 1; ncl < num_ctrl_lines; ncl++){
                    gate_cache.gateCacheMap[GateMetaData(U_label, ncl, false)] = matrixSqrt(gate_cache.gateCacheMap[GateMetaData(U_label, ncl-1, false)]);
                    gate_cache.gateCacheMap[GateMetaData(U_label, ncl, true)] = adjointMatrix(gate_cache.gateCacheMap[GateMetaData(U_label, ncl, false)]);
                }
            }

            /**
             * @brief Get the Map of cached gates. Keys are GateMetaData object, and values are the gate types defined by choice of Simulator
             * 
             * @return auto& 
             */
            GateCache<SimulatorType>& getCachedGates(){
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
             * @brief Decompose n-qubit controlled op into 1 and 2 qubit gates. Control indices can be in any specified location
             * 
             * @tparam Type ComplexDP or ComplexSP 
             * @param qReg Qubit register
             * @param ctrlIndices Vector of indices for control lines
             * @param qTarget Target qubit for the unitary matrix U
             * @param U Unitary matrix, U
             * @param depth Depth of recursion.
             * @param isPauliX To indicate if the unitary is a PauliX matrix.
             */
            void applyNQubitControl(SimulatorType& qSim, 
                    const std::vector<std::size_t> ctrlIndices,
                    const std::vector<std::size_t> auxIndices,
                    const unsigned int qTarget,
                    const GateMetaData gate_md,
                    const Mat2x2Type& U, 
                    const unsigned int depth)
            {
                //No safety checks; be aware of what is physically possible (qTarget not in control_indices)
                int local_depth = depth + 1;
                
                /*
                if( gate_cache.gateCacheMap.find(gate_md) == gate_cache.gateCacheMap.end() ){
                    std::cout << "I AM ADDING TO THE MAPS" << std::endl;
                    std::cout << gate_md << "\n";
                    //initialiseMaps(qSim, gate_md.labelGate, U, ctrlIndices.size()+1);
                    addToMaps(gate_md.labelGate, U, ctrlIndices.size() + 1);
                }*/
/*
                if(gate_cache.getGateSet().find(gate_md.labelGate) == gate_cache.getGateSet().end() ){
                    for(auto& a: gate_cache.getGateSet())
                        std::cout << "GS=" << a << std::endl;
                    std::cout << "I AM ADDING TO THE MAPS" << std::endl;
                    std::cout << gate_md << "\n";
                    addToMaps(gate_md.labelGate, U, ctrlIndices.size() + 1);
                }*/



                //Determine the range over which the qubits exist; consider as a count of the control ops, hence +1 since extremeties included
                std::size_t cOps = ctrlIndices.size();

//Care must be taken with the following decomposition routines. Attempt to use caching with 3 and 2+ opts only first
/*
                if( (cOps >= 6) && (auxIndices.size() >= 1) && (U.first == "X") && (depth == 0) ){
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

                    applyNQubitControl_CXOpt(qSim, subMCtrlIndicesNCX, subMAuxIndicesNCX, auxIndices[0], sqrtMatricesX[0], 0 );
                    applyNQubitControl_CXOpt(qSim, subLCtrlIndicesNCX, subLAuxIndicesNCX, qTarget, sqrtMatricesX[0], 0);
                    applyNQubitControl_CXOpt(qSim, subMCtrlIndicesNCX, subMAuxIndicesNCX, auxIndices[0], sqrtMatricesX[0], 0 );
                    applyNQubitControl_CXOpt(qSim, subLCtrlIndicesNCX, subLAuxIndicesNCX, qTarget, sqrtMatricesX[0], 0);
                }

                else if( (cOps == 5) && (auxIndices.size() >= 3) && (U.first == "X") && (depth == 0) ){ //161 -> 60 2-qubit gate calls

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
                }*/

                if(cOps == 3){ //Optimisation for replacing 17 with 13 2-qubit gate calls
                    //Create the two keys for accessign the appropriate matrices to perform the operations
                    const auto gmd = GateMetaData(gate_md.labelGate, local_depth+1, gate_md.adjoint, gate_md.theta);
                    const auto gmd_adj = GateMetaData(gate_md.labelGate, local_depth+1, !gate_md.adjoint, gate_md.theta);

                    //Apply the 13 2-qubit gate calls
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd], ctrlIndices[0], qTarget, gate_md.labelGate ); //Double check to see if gate is being referenced rather than copied
                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[1]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd_adj], ctrlIndices[1], qTarget, gate_md.labelGate );
                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[1]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd], ctrlIndices[1], qTarget, gate_md.labelGate );
                    qSim.applyGateCX( ctrlIndices[1], ctrlIndices[2]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd_adj], ctrlIndices[2], qTarget, gate_md.labelGate );
                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[2]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd], ctrlIndices[2], qTarget, gate_md.labelGate );
                    qSim.applyGateCX( ctrlIndices[1], ctrlIndices[2]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd_adj], ctrlIndices[2], qTarget, gate_md.labelGate );
                    qSim.applyGateCX( ctrlIndices[0], ctrlIndices[2]);
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd], ctrlIndices[2], qTarget, gate_md.labelGate );
                }

                else if (cOps >= 2 && cOps !=3){
                    std::vector<std::size_t> subCtrlIndices(ctrlIndices.begin(), ctrlIndices.end()-1);

                    const auto gmd = GateMetaData(gate_md.labelGate, local_depth, gate_md.adjoint, gate_md.theta);
                    const auto gmd_adj = GateMetaData(gate_md.labelGate, local_depth, !gate_md.adjoint, gate_md.theta);

                    //Apply single qubit gate ops, and decompose higher order controls further
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd], ctrlIndices.back(), qTarget, gate_md.labelGate ); //Double check to see if gate is being referenced rather than copied
                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, ctrlIndices.back(), GateMetaData("X"), qSim.getGateX(), 0 );
                    qSim.applyGateCU( gate_cache.gateCacheMap[gmd_adj], ctrlIndices.back(), qTarget, gate_md.labelGate );
                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, ctrlIndices.back(), GateMetaData("X"), qSim.getGateX(), 0 );
                    applyNQubitControl(qSim, subCtrlIndices, auxIndices, qTarget, gmd, gate_cache.gateCacheMap[gmd], local_depth );
                }

                //If the number of control qubits is less than 2, assume we have decomposed sufficiently
                else{
                    qSim.applyGateCU(gate_cache.gateCacheMap[gate_md], ctrlIndices[0], qTarget, gate_md.labelGate); //The first decomposed matrix value is used here
                }
            }

            /**
             * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
             * 
             * @tparam Type ComplexDP or ComplexSP
             * @param U Unitary matrix to be rooted
             * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
             */
            const Mat2x2Type matrixSqrt(const Mat2x2Type& U){
                Mat2x2Type V(U);
                std::complex<double> delta = U(0,0)*U(1,1) - U(0,1)*U(1,0);
                std::complex<double> tau = U(0,0) + U(1,1);
                std::complex<double> s = sqrt(delta);
                std::complex<double> t = sqrt(tau + 2.0*s);

                //must be a way to vectorise these; TinyMatrix have a scale/shift option?
                V(0,0) += s;
                V(1,1) += s;
                std::complex<double> scale_factor(1.,0.);
                scale_factor /= t;
                V(0,0) *= scale_factor; //(std::complex<double>(1.,0.)/t);
                V(0,1) *= scale_factor; //(1/t);
                V(1,0) *= scale_factor; //(1/t);
                V(1,1) *= scale_factor; //(1/t);

                return V;
            }

            /**
             * @brief Function to calculate the adjoint of an input matrix
             * 
             * @tparam Type ComplexDP or ComplexSP
             * @param U Unitary matrix to be adjointed
             * @return openqu::TinyMatrix<Type, 2, 2, 32> U^{\dagger}
             */
            Mat2x2Type adjointMatrix(const Mat2x2Type& U){
                Mat2x2Type Uadjoint(U);
                std::complex<double> tmp;
                tmp = Uadjoint(0,1);
                Uadjoint(0,1) = Uadjoint(1,0);
                Uadjoint(1,0) = tmp;
                Uadjoint(0,0) = std::conj(Uadjoint(0,0));
                Uadjoint(0,1) = std::conj(Uadjoint(0,1));
                Uadjoint(1,0) = std::conj(Uadjoint(1,0));
                Uadjoint(1,1) = std::conj(Uadjoint(1,1));
                return Uadjoint;
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
