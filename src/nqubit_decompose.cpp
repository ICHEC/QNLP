/**
 * @file nqubit_decompose.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Functions for applying n-qubit controlled U gates
 * @version 0.1
 * @date 2019-03-06
 */

//#define __VERBOSE__ 1
//#define __VERBOSE_KEYS__ 1
#ifdef __VERBOSE__
#include <string>
static unsigned int op_counter = 0;
#endif

#include "nqubit_decompose.hpp"
using namespace QNLP;

template <class Type>
NQubitDecompose<Type>::NQubitDecompose(){ }

template <class Type>
NQubitDecompose<Type>::NQubitDecompose(openqu::TinyMatrix<Type, 2, 2, 32> U, std::size_t num_ctrl_gates){
    NQubitDecompose<Type>::initialiseMaps(U, num_ctrl_gates);
}

template <class Type>
NQubitDecompose<Type>::~NQubitDecompose(){
    NQubitDecompose<Type>::clearMaps();
}

/**
 * @brief Clears the maps of stored sqrt matrices
 * 
 */
template <class Type>
void NQubitDecompose<Type>::clearMaps(){
    NQubitDecompose<Type>::sqrtMatricesU.clear();
    NQubitDecompose<Type>::sqrtMatricesX.clear();
}

/**
  *@brief Add the PauliX and the given unitary U to the maps, both indexed with key 0
 * 
 * @param U Unitary matrix
 */
template <class Type>
void NQubitDecompose<Type>::initialiseMaps(openqu::TinyMatrix<Type, 2, 2, 32> U, std::size_t num_ctrl_lines){
    openqu::TinyMatrix<Type, 2, 2, 32> px;
    px(0, 0) = Type(0., 0.);
    px(0, 1) = Type(1., 0.);
    px(1, 0) = Type(1., 0.);
    px(1, 1) = Type(0., 0.);
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
 * @brief Decompose n-qubit controlled op into 1 and 2 qubit gates. Requires the control register qubits to be adjacent. 
 * 
 * @tparam Type ComplexDP or ComplexSP 
 * @param qReg Qubit register
 * @param qControlStart First index of control qubits. 
 * @param qControlEnd Last index of control qubits. 
 * @param qTarget Target qubit for the unitary matrix U
 * @param U Unitary matrix, U
 * @param depth Depth of recursion.
 * @param isPauliX To indicate if the decomposition is of a PauliX matrix.
 */
template <class Type>
void NQubitDecompose<Type>::applyNQubitControl(QubitRegister<ComplexDP>& qReg, 
                    unsigned int qControlStart,
                    unsigned int qControlEnd,
                    unsigned int qTarget,
                    openqu::TinyMatrix<Type, 2, 2, 32>& U, 
                    unsigned int depth, bool isPauliX)
{
    //Some safety checks
    assert (qControlStart <= qControlEnd);
    assert (qReg.num_qubits >= qControlEnd);
    assert (qReg.num_qubits >= qTarget);
    assert (qControlEnd != qTarget);

    int local_depth = depth + 1;
    if(sqrtMatricesU.size() == 0){
        initialiseMaps(U, qControlEnd - qControlStart +1);
    }

    //Determine the range over which the qubits exist; consider as a count of the control ops, hence +1 since extremeties included
    std::size_t cOps = qControlEnd - qControlStart + 1;

#ifdef __VERBOSE__
    std::cout << "cOps[" << local_depth << "]:=" << cOps << std::endl;
#endif

    openqu::TinyMatrix<Type, 2, 2, 32> op, opAdj;

    if (cOps >= 2){
        //The input matrix to be decomposed can be either a PauliX, or arbitrary unitary. Separated, as the Pauli decomposition can be built from phase modifications directly.
        if ( ! isPauliX ){
            op = sqrtMatricesU[local_depth];
            opAdj = sqrtMatricesU[-local_depth];
        }
        else {
            op = sqrtMatricesX[local_depth];
            opAdj = sqrtMatricesX[-local_depth];
        }

        //Apply single qubit gate ops, and decompose higher order controls further
        qReg.ApplyControlled1QubitGate(qControlEnd, qTarget, op);

#ifdef __VERBOSE__
        std::cout << "################################################" << std::endl;
        op.print("OP");
        std::cout << op_counter <<". Applied op between " << qControlEnd <<" and " << qTarget << std::endl;
        std::cout << "################################################" << std::endl;
        op_counter++;
#endif

        applyNQubitControl(qReg, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0], 0, true );

        qReg.ApplyControlled1QubitGate(qControlEnd, qTarget, opAdj);

#ifdef __VERBOSE__
        std::cout << "################################################" << std::endl;
        opAdj.print("OPADJ");
        std::cout << op_counter << ". Applied op between " << qControlEnd <<" and " << qTarget << std::endl;
        std::cout << "################################################" << std::endl;
        op_counter++;
#endif

        applyNQubitControl(qReg, qControlStart, qControlEnd-1, qControlEnd, sqrtMatricesX[0],   0,              true );

        applyNQubitControl(qReg, qControlStart, qControlEnd-1, qTarget,     matrixSqrt(U),      local_depth,    false );
    }

    //If the number of control qubits is less than 2, assume we have decomposed sufficiently
    else{
        op = U; 
        qReg.ApplyControlled1QubitGate(qControlEnd, qTarget, op); //The first decomposed matrix value is used here
#ifdef __VERBOSE__
        std::cout << "################################################" << std::endl;
        op.print("U");
        std::cout << op_counter << ". Applied op between " << qControlEnd <<" and " << qTarget << std::endl;
        std::cout << "################################################" << std::endl;
        op_counter++;
#endif
    }
#ifdef __VERBOSE_KEYS__
    std::cout << "################################################" << std::endl;
    std::cout << "KEYS U" << std::endl;
    for (auto &a : sqrtMatricesU){
        a.second.print(std::to_string(a.first));
    }
    std::cout << "################################################" << std::endl;
    std::cout << "KEYS X" << std::endl;
    for (auto &a : sqrtMatricesX){
        a.second.print(std::to_string(a.first));
    }
    std::cout << "################################################" << std::endl;
#endif
}

/**
 * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
 * 
 * @tparam Type ComplexDP or ComplexSP
 * @param U Unitary matrix to be rooted
 * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
 */
template <class Type>
openqu::TinyMatrix<Type, 2, 2, 32> NQubitDecompose<Type>::matrixSqrt(const openqu::TinyMatrix<Type, 2, 2, 32>& U){
    openqu::TinyMatrix<Type, 2, 2, 32> V(U);
    Type delta = U(0,0)*U(1,1) - U(0,1)*U(1,0);
    Type tau = U(0,0) + U(1,1);
    Type s = sqrt(delta);
    Type t = sqrt(tau + 2.0*s);

#ifdef __VERBOSE__
    std::cout << "#############################" << std::endl;
    std::cout << "delta=" << delta << std::endl;
    std::cout << "tau=" << tau << std::endl;
    std::cout << "s=" << s << std::endl;
    std::cout << "t=" << t << std::endl;
    std::cout << "#############################" << std::endl;
#endif

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
template <class Type>
openqu::TinyMatrix<Type, 2, 2, 32> NQubitDecompose<Type>::adjointMatrix(const openqu::TinyMatrix<Type, 2, 2, 32>& U){
    openqu::TinyMatrix<Type, 2, 2, 32> Uadjoint(U);
    Type tmp;
    tmp = Uadjoint(0,0);
    Uadjoint(0,0) = Uadjoint(1,1);
    Uadjoint(1,1) = tmp;

    Uadjoint(0,0) = std::conj(Uadjoint(0,0));
    Uadjoint(0,1) = std::conj(Uadjoint(0,1));
    Uadjoint(1,0) = std::conj(Uadjoint(1,0));
    Uadjoint(1,1) = std::conj(Uadjoint(1,1));

    return Uadjoint;
}

