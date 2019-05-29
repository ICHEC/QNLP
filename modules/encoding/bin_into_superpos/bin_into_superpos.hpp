/**
 * @file bin_into_superpos.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Functions for encoding binary numbers into a superposition of states
 * @version 0.1
 * @date 2019-29-06
 */

#ifndef QNLP_BINENCODE
#define QNLP_BINENCODE

//#include "Simulator.hpp"
#include <complex>
#include <cassert>
#include <utility>

namespace QNLP{

    /**
     * @brief Checks if the bit'th bit of the integer byte is set
     * 
     * @param byte - integer representing a binary string
     * @param bit - the index of the bit to be checked (beginning
     * with the least significant bit)
     */
    #define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

    template <class SimulatorType>
    class EncodeBinIntoSuperpos{
        private:
            //Take the 2x2 matrix type from the template SimulatorType
            using Mat2x2Type = decltype(std::declval<SimulatorType>().getGateX());

            Mat2x2Type pX;
            std::unique_ptr< std::vector<Mat2x2Type> > S;

            unsigned m, len_reg_ancilla, len_bit_pattern;
    


        public:
            EncodeBinIntoSuperpos(){};

            EncodeBinIntoSuperpos(const std::size_t num_bin_patterns, const std::size_t len_bit_pattern_){
                m = num_bin_patterns;
                len_bit_pattern = len_bit_pattern_;

                initialiseMats();
            };

            ~EncodeBinIntoSuperpos(){
                clearMats();    
            };

            /**
             * @brief Define the PauliX and the unitary matrix S
             * 
             */
            //template <class Type>
            void initialiseMats(){
                pX(0, 0) = std::complex<double>( 0., 0. );
                pX(0, 1) = std::complex<double>( 1., 0. );
                pX(1, 0) = std::complex<double>( 1., 0. );
                pX(1, 1) = std::complex<double>( 0., 0. );

                // Preparation for binary encoding:
                //
                // Prepare three matrices S^1,S^2,...,S^m that are required for the implemented
                // algorithm to encode these binary strings into a superposition state.
                //
                // Note the matrix indexing of the S vector of S^p matrices will be backwards:
                //      S[0] -> S^p
                //      S[1] -> S_{p-1}, and so on.
                S = new std::vector<Mat2x2Type> (m, Mat2x2Type);
                {
                    int p = m;
                    double diag, off_diag;

                    for(int i = 0; i < m; i++){
                        off_diag = 1.0/sqrt((double)(p));
                        diag = off_diag * sqrt((double)(p-1));

                        (*S)[i](0,0) = {diag, 0.0};
                        (*S)[i](0,1) = {off_diag, 0.0};
                        (*S)[i](1,0) = {-off_diag, 0.0};
                        (*S)[i](1,1) = {diag, 0.0};

                        p--;
                    }
                }
            }

            /**
             * @brief Clears the S matricess 
             * 
             */
            void clearMats(){
                S.clear();
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



            void encodeBinInToSuperpos(SimulatorType& qSim, const std::vector<unsigned>& reg_memory, const std::vector<unsigned>& reg_ancilla, const std::vector<unsigned>& bin_patterns){

                int len_reg_ancilla;
                len_reg_ancilla = reg_ancilla.size();

                // Require length of ancilla register to have n+2 qubits
                assert(m + 1 < len_reg_ancilla);


                // Prepare state in |0...>|0...0>|10> of lengths n,n,2
                qSim.ApplyPauliX(reg_ancilla[len_reg_ancilla-1]);

                // Begin Encoding
                for(int i = 0; i < m; i++){
                    // Psi0
                    // Encode inputted binary pattern to pReg
                    for(int j = 0; j < len_bin_pattern; j++){
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.ApplyPauliX(reg_ancilla[j]);
                        }
                    }

                    // Psi1
                    // Encode inputted binary pattern
                    for(int j = 0; j < len_bin_pattern; j++){
                        qSim.ApplyToffoli(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Psi2
                    for(int j = 0; j < len_bin_pattern; j++){
                        qSim.ApplyCPauliX(reg_ancilla[j], reg_memory[j]);
                        qSim.ApplyPauliX(reg_memory[j]);
                    }

                    // Psi3
                    op_nCDecomp->applyNQubitControl(*this, reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2], *X, 0, true);

                    // Psi4
                    // Step 1.3 - Apply S^i
                    // This flips the second control bit of the new term in the position so
                    // that we get old|11> + new|10>
                    // Break off into larger and smaller chunks
                    qSim.ApplyControlled1QubitGate(reg_ancilla[len_reg_ancilla-2], reg_ancilla[len_reg_ancilla-1], (*S)[i]);

                    // Psi5
                    op_nCDecomp->applyNQubitControl(*this, reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2], *X, 0, true);

                    // Psi6 
                    for(int j = len_bin_pattern-1; j > -1; j--){
                        qSim.ApplyPauliX(reg_memory[j]);
                        qSim.ApplyCPauliX(reg_ancilla[j], reg_memory[j]);
                    }

                    // Psi7
                    for(int j = len_bin_pattern-1; j > -1; j--){
                        qSim.ApplyToffoli(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
                    }

                    // Reset the p register of the new term to the state |0...0>
                    for(int j = 0; j < len_bin_pattern; j++){
                        // Check current pattern against next pattern
                        if(IS_SET(bin_patterns[i],j)){
                            qSim.ApplyPauliX(reg_ancilla[j]);
                        }

                    }
                }

            }
    };

};
#endif


