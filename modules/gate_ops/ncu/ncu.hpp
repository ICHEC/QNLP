/**
 * @file ncu.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Functions for applying n-qubit controlled U (unitary) gates
 * @version 0.1
 * @date 2019-03-06
 */

#ifndef QNLP_NCU
#define QNLP_NCU

//#include "Simulator.hpp"
#include <unordered_map>
#include <complex>
#include <cassert>
#include <utility>
#ifdef LATEX_OUTPUT
    #include <iostream>
    #include <fstream>
#endif

namespace QNLP{
    template <class SimulatorType>
    class NCU{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());
            // Maintains a table of pre-calculated matrices to avoid repeated work. Absolute value of the key gives the depth of the sqrt ( U^{1/2^n} ), and the sign indicates if it is the adjoint (negative). Key is the depth of the operation, and value is a pair containing the gate label and numeric matrix
            std::unordered_map<int, std::pair<std::string, Mat2x2Type> > sqrtMatricesU {};
            std::unordered_map<int, std::pair<std::string, Mat2x2Type> > sqrtMatricesX {};
            static std::size_t num_gate_ops;
            #ifdef LATEX_OUTPUT
                std::ofstream latex_csv_out, matrix_val_out;
            #endif

        public:
            NCU(){
                #ifdef LATEX_OUTPUT
                    latex_csv_out.open ("ncu.csv");
                    matrix_val_out.open("ncu_matrices.log");
                #endif
            };
            NCU(std::string matrixLabel, const Mat2x2Type& U, const std::size_t num_ctrl_gates){
                initialiseMaps(matrixLabel, U, num_ctrl_gates);
                #ifdef LATEX_OUTPUT
                    latex_csv_out.open ("ncu.csv");
                    matrix_val_out.open("ncu_matrices.log");
                #endif
            };

            ~NCU(){
                clearMaps();
                #ifdef LATEX_OUTPUT
                    latex_csv_out.close();
                    matrix_val_out.close();
                #endif
            };

            /**
             * @brief Add the PauliX and the given unitary U to the maps, both indexed with key 0
             * 
             * @param U 
             */
            //template <class Type>
            void initialiseMaps(std::string U_label, const Mat2x2Type& U, const std::size_t num_ctrl_lines){
                Mat2x2Type px;
                px(0, 0) = std::complex<double>( 0., 0. );
                px(0, 1) = std::complex<double>( 1., 0. );
                px(1, 0) = std::complex<double>( 1., 0. );
                px(1, 1) = std::complex<double>( 0., 0. );

                sqrtMatricesX[0] = std::make_pair("X", px);
                //sqrtMatricesX[1] = matrixSqrt("X", px);
                //sqrtMatricesX[-1] = std::make_pair("X", adjointMatrix(sqrtMatricesX[1].second) );
                sqrtMatricesU[0] = std::make_pair(U_label, U);

                for(std::size_t ncl = 1; ncl < num_ctrl_lines; ncl++){
                    sqrtMatricesX[ncl] = matrixSqrt(sqrtMatricesX[ncl-1].first, sqrtMatricesX[ncl-1].second);
                    sqrtMatricesX[-ncl] = std::make_pair(sqrtMatricesX[ncl].first, adjointMatrix(sqrtMatricesX[ncl].second));

                    sqrtMatricesU[ncl] = matrixSqrt(sqrtMatricesU[ncl-1].first, sqrtMatricesU[ncl-1].second);
                    sqrtMatricesU[-ncl] = std::make_pair(sqrtMatricesU[ncl].first, adjointMatrix(sqrtMatricesU[ncl].second) );
                }
                #ifdef LATEX_OUTPUT
                    matrix_val_out << U_label << std::endl;
                    for(auto& a: sqrtMatricesU)
                        matrix_val_out << a.first << "   " << a.second.second.tostr() << std::endl;
                    matrix_val_out << "X" << std::endl;
                    for(auto& a: sqrtMatricesX)
                        matrix_val_out << a.first << "   " << a.second.second.tostr() << std::endl;
                #endif

            }

            /**
             * @brief Clears the maps of stored sqrt matrices
             * 
             */
            //template <class Type>
            void clearMaps(){
                sqrtMatricesU.clear();
                sqrtMatricesX.clear();
            }

            /**
             * @brief Decompose n-qubit controlled op into 1 and 2 qubit gates. Requires the control register qubits to be adjacent. 
             * 
             * @tparam Type ComplexDP or ComplexSP 
             * @param qReg Qubit register
             * @param qControlStart First index of control qubits. 
             * @param qControlEnd Last index of control qubits. 
             * @param qTarget Target qubit for the unitary matrix U
             * @param U Unitary matrix, U
             * @param depth Depth of recursion.
             * @param isPauliX To indicate if the unitary is a PauliX matrix.
             */
            void applyNQubitControl(SimulatorType& qSim, 
                    const unsigned int qControlStart,
                    const unsigned int qControlEnd,
                    const unsigned int qTarget,
                    const std::pair<std::string, Mat2x2Type>& U, 
                    const unsigned int depth)
            {
                //Some safety checks
                assert (qControlStart <= qControlEnd);
                assert (qSim.getNumQubits() >= qControlEnd);
                assert (qSim.getNumQubits() >= qTarget);
                assert (qControlEnd != qTarget);

                int local_depth = depth + 1;
                if(sqrtMatricesU.size() == 0){
                    initialiseMaps(U.first, U.second, qControlEnd - qControlStart +1);
                }

                //Determine the range over which the qubits exist; consider as a count of the control ops, hence +1 since extremeties included
                std::size_t cOps = qControlEnd - qControlStart + 1;

                std::unordered_map<int, std::pair<std::string, Mat2x2Type> > *map_ptr;

                if (cOps >= 2){
                    //The input matrix to be decomposed can be either a PauliX, or arbitrary unitary. Separated, as the Pauli decomposition can be built from phase modifications directly.
                    if ( ! (U.first == "X") ){
                        map_ptr = &sqrtMatricesU;
                    }
                    else {
                        map_ptr = &sqrtMatricesX;
                    }
                    //Apply single qubit gate ops, and decompose higher order controls further
                    qSim.applyGateCU( (*map_ptr)[local_depth].second, qControlEnd, qTarget );
                    #ifdef LATEX_OUTPUT
                        latex_csv_out << U.first << "[" << local_depth << "]," << qControlEnd << "," << qTarget << "," << (*map_ptr)[local_depth].second.tostr() << std::endl;
                    #endif
                    applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0 );

                    qSim.applyGateCU( (*map_ptr)[-local_depth].second, qControlEnd, qTarget);
                    #ifdef LATEX_OUTPUT
                        latex_csv_out << U.first << "[" << -local_depth << "]," << qControlEnd << "," << qTarget << "," << (*map_ptr)[-local_depth].second.tostr() << std::endl;
                    #endif
                    applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0 );

                    decltype(auto) sqrt_U(matrixSqrt(U.first, U.second));
                    applyNQubitControl(qSim, qControlStart, qControlEnd-1, qTarget, sqrt_U, local_depth );

                    //Reset pointer
                    map_ptr = nullptr;
                }

                //If the number of control qubits is less than 2, assume we have decomposed sufficiently
                else{
                    qSim.applyGateCU(U.second, qControlEnd, qTarget); //The first decomposed matrix value is used here
                    #ifdef LATEX_OUTPUT
                        latex_csv_out << U.first << "[" << depth << "]," << qControlEnd << "," << qTarget << ","  << U.second.tostr() << std::endl;
                    #endif
                }
            }

            /**
             * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
             * 
             * @tparam Type ComplexDP or ComplexSP
             * @param U Unitary matrix to be rooted
             * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
             */
            std::pair<std::string, const Mat2x2Type> matrixSqrt(std::string label, const Mat2x2Type& U){
                Mat2x2Type V(U);
                std::complex<double> delta = U(0,0)*U(1,1) - U(0,1)*U(1,0);
                std::complex<double> tau = U(0,0) + U(1,1);
                std::complex<double> s = sqrt(delta);
                std::complex<double> t = sqrt(tau + 2.0*s);

                //must be a way to vectorise these; TinyMatrix have a scale/shift option?
                V(0,0) += s;
                V(1,1) += s;
                std::complex<double> scale_factor(1.,0.);
                scale_factor/=t;
                V(0,0) *= scale_factor; //(std::complex<double>(1.,0.)/t);
                V(0,1) *= scale_factor; //(1/t);
                V(1,0) *= scale_factor; //(1/t);
                V(1,1) *= scale_factor; //(1/t);

                return std::make_pair(label, V);
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
    };

};
#endif
