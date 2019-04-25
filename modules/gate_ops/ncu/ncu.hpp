/**
 * @file ncu.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Functions for applying n-qubit controlled U (unitary) gates
 * @version 0.1
 * @date 2019-03-06
 */

#ifndef QNLP_NCU
#define QNLP_NCU

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <unordered_map>

namespace QNLP{

template <class Type>
class NCU{
    private:
        // Maintains a table of pre-calculated matrices to avoid repeated work. Absolute value of the key gives the depth of the sqrt ( U^{1/2^n} ), and the sign indicates if it is the adjoint (negative).
        std::unordered_map<int, openqu::TinyMatrix<Type, 2, 2, 32>> sqrtMatricesU {};
        std::unordered_map<int, openqu::TinyMatrix<Type, 2, 2, 32>> sqrtMatricesX {};

    public:
    NCU();
    NCU(openqu::TinyMatrix<Type, 2, 2, 32> U, std::size_t num_ctrl_gates);

    ~NCU();

    /**
     * @brief Add the PauliX and the given unitary U to the maps, both indexed with key 0
     * 
     * @param U 
     */
    //template <class Type>
    void initialiseMaps(openqu::TinyMatrix<Type, 2, 2, 32> U, std::size_t num_ctrl_lines);



    /**
     * @brief Clears the maps of stored sqrt matrices
     * 
     */
    //template <class Type>
    void clearMaps();

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
    //template <class Type>
    void applyNQubitControl(QubitRegister<ComplexDP>& qReg, 
                        unsigned int qControlStart,
                        unsigned int qControlEnd,
                        unsigned int qTarget,
                        openqu::TinyMatrix<Type, 2, 2, 32>& U, 
                        unsigned int depth, bool isPauliX);

    /**
     * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
     * 
     * @tparam Type ComplexDP or ComplexSP
     * @param U Unitary matrix to be rooted
     * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
     */
    //template <class Type>
    openqu::TinyMatrix<Type, 2, 2, 32> matrixSqrt(const openqu::TinyMatrix<Type, 2, 2, 32>& U);

    /**
     * @brief Function to calculate the adjoint of an input matrix
     * 
     * @tparam Type ComplexDP or ComplexSP
     * @param U Unitary matrix to be adjointed
     * @return openqu::TinyMatrix<Type, 2, 2, 32> U^{\dagger}
     */
    //template <class Type>
    openqu::TinyMatrix<Type, 2, 2, 32> adjointMatrix(const openqu::TinyMatrix<Type, 2, 2, 32>& U);

};

//Instantiate the allowed class types
template class NCU<ComplexSP>;
template class NCU<ComplexDP>;

};
#endif
