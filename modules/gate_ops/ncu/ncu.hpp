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

namespace QNLP{
    template <class SimulatorType>
    class NCU{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());
            // Maintains a table of pre-calculated matrices to avoid repeated work. Absolute value of the key gives the depth of the sqrt ( U^{1/2^n} ), and the sign indicates if it is the adjoint (negative).
            std::unordered_map<int, Mat2x2Type> sqrtMatricesU {};
            std::unordered_map<int, Mat2x2Type> sqrtMatricesX {};

        public:
            NCU(){};
            NCU(const Mat2x2Type& U, const std::size_t num_ctrl_gates){
                initialiseMaps(U, num_ctrl_gates);
            };

            ~NCU(){
                clearMaps();    
            };

            /**
             * @brief Add the PauliX and the given unitary U to the maps, both indexed with key 0
             * 
             * @param U 
             */
            //template <class Type>
            void initialiseMaps(const Mat2x2Type& U, const std::size_t num_ctrl_lines){
                Mat2x2Type px;
                px(0, 0) = std::complex<double>( 0., 0. );
                px(0, 1) = std::complex<double>( 1., 0. );
                px(1, 0) = std::complex<double>( 1., 0. );
                px(1, 1) = std::complex<double>( 0., 0. );

                sqrtMatricesX[0] = px;
                sqrtMatricesX[1] = matrixSqrt(px);
                sqrtMatricesX[-1] = adjointMatrix(sqrtMatricesX[1]);
                sqrtMatricesU[0] = U;

                for(std::size_t ncl = 1; ncl < num_ctrl_lines; ncl++){
                    if(ncl+1 < num_ctrl_lines){
                        sqrtMatricesX[ncl] = matrixSqrt(sqrtMatricesX[ncl-1]);
                        sqrtMatricesX[-ncl] = adjointMatrix(sqrtMatricesX[ncl]);
                    }
                    sqrtMatricesU[ncl] = matrixSqrt(sqrtMatricesU[ncl-1]);
                    sqrtMatricesU[-ncl] = adjointMatrix(sqrtMatricesU[ncl]);
                }

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
                    const Mat2x2Type& U, 
                    const unsigned int depth, const bool isPauliX)
            {
                //Some safety checks
                assert (qControlStart <= qControlEnd);
                assert (qSim.getNumQubits() >= qControlEnd);
                assert (qSim.getNumQubits() >= qTarget);
                assert (qControlEnd != qTarget);

                int local_depth = depth + 1;
                if(sqrtMatricesU.size() == 0){
                    initialiseMaps(U, qControlEnd - qControlStart +1);
                }

                //Determine the range over which the qubits exist; consider as a count of the control ops, hence +1 since extremeties included
                std::size_t cOps = qControlEnd - qControlStart + 1;

                if (cOps >= 2){
                    //The input matrix to be decomposed can be either a PauliX, or arbitrary unitary. Separated, as the Pauli decomposition can be built from phase modifications directly.
                    if ( ! isPauliX ){
                        //Apply single qubit gate ops, and decompose higher order controls further
                        qSim.applyGateCU( sqrtMatricesU[local_depth], qControlEnd, qTarget );
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0, true );

                        qSim.applyGateCU(sqrtMatricesU[-local_depth], qControlEnd, qTarget);
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0, true );

                        Mat2x2Type sqrt_U(matrixSqrt(U));
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qTarget, sqrt_U, local_depth, false );
                    }
                    else {
                        //Apply single qubit gate ops, and decompose higher order controls further
                        qSim.applyGateCU(sqrtMatricesX[local_depth], qControlEnd, qTarget);
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0, true );

                        qSim.applyGateCU(sqrtMatricesX[-local_depth], qControlEnd, qTarget);
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0, true );

                        Mat2x2Type sqrt_U(matrixSqrt(U));
                        applyNQubitControl(qSim, qControlStart, qControlEnd-1, qTarget, sqrt_U, local_depth, false );
                    }
                }

                //If the number of control qubits is less than 2, assume we have decomposed sufficiently
                else{
                    qSim.applyGateCU(U, qControlEnd, qTarget); //The first decomposed matrix value is used here
                }
            }

            /**
             * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
             * 
             * @tparam Type ComplexDP or ComplexSP
             * @param U Unitary matrix to be rooted
             * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
             */
            Mat2x2Type matrixSqrt(const Mat2x2Type& U){
                Mat2x2Type V(U);
                std::complex<double> delta = U(0,0)*U(1,1) - U(0,1)*U(1,0);
                std::complex<double> tau = U(0,0) + U(1,1);
                std::complex<double> s = sqrt(delta);
                std::complex<double> t = sqrt(tau + 2.0*s);

                //must be a way to vectorise these; TinyMatrix have a scale/shift option?
                V(0,0) += s;
                V(1,1) += s;

                V(0,0) *= (1/t);
                V(0,1) *= (1/t);
                V(1,0) *= (1/t);
                V(1,1) *= (1/t);

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
                tmp = Uadjoint(0,0);
                Uadjoint(0,0) = Uadjoint(1,1);
                Uadjoint(1,1) = tmp;
                Uadjoint(0,0) = std::conj(Uadjoint(0,0));
                Uadjoint(0,1) = std::conj(Uadjoint(0,1));
                Uadjoint(1,0) = std::conj(Uadjoint(1,0));
                Uadjoint(1,1) = std::conj(Uadjoint(1,1));
                return Uadjoint;
            }

    };

};
#endif
