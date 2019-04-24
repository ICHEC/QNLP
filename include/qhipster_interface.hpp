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

    ~IntelSimulator(){
        //TODO: ensure everything is safely freed here
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

    // Defining gate operations
    void applyGate(std::array<ComplexDP>& gate){
        TODO
    }

    // Defining gate operations
    void applyGate(std::string gateName){
        TODO
    }

    const TinyMatrix& getGate(std::string gateName) const {
        TODO 
        //May be difficult to generalise
    }

    void applyGateSqrtX(std::size_t qubitIndex){
        qubitRegister.applyPauliSqrtX(std::size_t qubitIndex);
    };

    //Arbitrary rotation along axis; given in radians
    void applyGateRotX(std::size_t qubitIndex) {
        qubitRegister.applyRotationX(qubitIndex);
    };
    void applyGateRotY(std::size_t qubitIndex) {
        qubitRegister.applyRotationY(qubitIndex);
    };
    void applyGateRotZ(std::size_t qubitIndex) {
        qubitRegister.applyRotationZ(qubitIndex);
    };

    //2 qubit gates
    void applyGateCU(std::size_t control, std::size_t target, TinyMatrix<ComplexDP, 2, 2, 32> & U){
        qubitRegister.applyControlled1QubitGate(control, target, U);
    }

    void applyGateCX(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliX(control, target);
    }
    void applyGateCY(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliY(control, target);
    }
    void applyGateCZ(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliZ(control, target);
    }
    void applyGateCH(const std::size_t control, const std::size_t target){
        qubitRegister.applyCHadamard(control, target);
    }

    void applyGateCRotX(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationX(control, target, theta);
    }
    void applyGateCRotY(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationY(control, target, theta);
    }
    void applyGateCRotZ(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationZ(control, target, theta);
    }

    void applyGateSwap(std::size_t q1, std::size_t q2){
        qubitRegister.applySwap(q1, q2);
    }
    void applyGateSqrtSwap(){
        qubitRegister.applySqrtISwap
    }
    void applyGatePhaseShift(){
        //qubitRegister.apply
    }

    //3 qubit gates
    virtual void applyGateToffoli();
    virtual void applyGateFredkin();

    //Defining Qubit operations
    QubitRegisterType getQubitRegister() { return this->qubitRegister; }
    const QubitRegisterType& getQubitRegister() const { return this->qubitRegister; };

    virtual QubitType getQubit();
    virtual const QubitType& getQubit() const;

    std::size_t getNumQubits() { return numQubits; }

};

#endif
