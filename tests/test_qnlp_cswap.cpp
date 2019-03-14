/**
 * @file test_qnlp_cswap.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "../include/sim_interface.hpp"
#include "../include/sim_factory.hpp"
#include <memory>

using namespace QNLP;

void test(int sim, std::size_t numQubits){
    auto s = SimulatorInterface::createSimulator(sim, numQubits);
}

/*
 * Controlled swap decomposition taken from arXiV:1301.3727
 */
template <class QubitReg>
void ApplyControlledSwap(   QubitReg &qreg, 
                            unsigned int idx_q0, 
                            unsigned int idx_q1, 
                            unsigned int idx_q2 ){

    //Need at least 3 qubits for this operation
    std::cout << "NUMQBITS = " << qreg.num_qubits << std::endl;
    std::cout << "q0 = " << idx_q0 << "    q1 = "  << idx_q1 << "   q2 = " << idx_q2 << std::endl; 

    assert( qreg.num_qubits > 2 );
    //The requested qubit indices must be available to use within the register range
    assert( (idx_q0 < qreg.num_qubits ) && (idx_q1 < qreg.num_qubits) && (idx_q2 < qreg.num_qubits) );
    //The qubits must be different from one another
    assert( idx_q0 != idx_q1 && idx_q0 != idx_q2 && idx_q1 != idx_q2);

    //V = sqrt(X)
    /*
    openqu::TinyMatrix<Type, 2, 2, 32> V;
    V(0,0) = {0.5,  0.5};
    V(0,1) = {0.5, -0.5};
    V(1,0) = {0.5, -0.5};
    V(1,1) = {0.5,  0.5};
    */

    //Indexing over LSB as above, with alternating real/img components.
    std::array<double> V = {0.5,0.5, 0.5,-0.5, 0.5,0.5, 0.5,0.5};
    std::array<double> V_dag = {0.5,-0.5, 0.5,0.5, 0.5,0.5, 0.5,-0.5};

    // Consider creating a DB of gates, populated as they are generated.
    // Try to extract from DB, create if non existant.



    
/*
    openqu::TinyMatrix<Type, 2, 2, 32> V_dag;
    V_dag(0,0) = {0.5, -0.5};
    V_dag(0,1) = {0.5,  0.5};
    V_dag(1,0) = {0.5,  0.5};
    V_dag(1,1) = {0.5, -0.5};
*/




    qreg.ApplyCPauliX(q2, q1);
    qreg.ApplyControlled1QubitGate(q1, q2, V);
    qreg.ApplyControlled1QubitGate(q0, q2, V);
    
    qreg.ApplyCPauliX(q0, q1);
    qreg.ApplyControlled1QubitGate(q1, q2, V_dag);
    qreg.ApplyCPauliX(q2, q1);
    qreg.ApplyCPauliX(q0, q1);
}