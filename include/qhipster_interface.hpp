//##############################################################################
/**
 *  @file    qhipster_interface.hpp
 *  @author  Lee J. O'Riordan
 *  @date    25/01/2019
 *  @version 0.1
 *
 *  @brief Intel qHiPSTER simulator interface.
 *
 *  @section DESCRIPTION
 *  This class implements a mapping between the QNLP code and the 
 *  Intel qHiPSTER simulator.
 * 
 */
//##############################################################################

#ifndef QNLP_QHIPSTER_INTERFACE_H
#define QNLP_QHIPSTER_INTERFACE_H

#include "sim_interface.hpp"
#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"

using namespace QNLP;

class IntelSimulator : SimulatorInterface< QbitRegister<ComplexDP>, QbitRegister<ComplexDP>> {

    IntelSimulator(int numQubits) : numQubits(numQubits), qubitRegister(numQubits, "base", 0){
    }

    void applyGateX(std::size_t qubitIndex){
        qubitRegister.applyPauliX(qubitIndex);
    }
    void applyGateY(std::size_t qubitIndex){
        qubitRegister.applyPauliY(qubitIndex);
    }
    void applyGateZ(std::size_t qubitIndex){
        qubitRegister.applyPauliZ(qubitIndex);
    }
    void applyGateI(std::size_t qubitIndex){
        qubitRegister.applyPauliX(qubitIndex);
    }
    void applyGateH(std::size_t qubitIndex){
        qubitRegister.applyPauliY(qubitIndex);
    }
};

#endif
