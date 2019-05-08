//##############################################################################
/**
 *  @file    intel_simulator.cpp
 *  @author  Lee J. O'Riordan
 *  @date    25/01/2019
 *  @version 0.1
 *
 *  @brief Intel QS interface layer.
 *
 *  @section DESCRIPTION
 *  This class implements a mapping between the QNLP code and the 
 *  Intel Quantum Simulator.
 * 
 */
//##############################################################################

#include "sim_interface.hpp"
#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"

using namespace QNLP;

class IntelSimulator : SimulatorInterface< QbitRegister<ComplexDP>, QbitRegister<ComplexDP>> {
    typedef const std::size_T CST;
    IntelSimulator(int numQubits) : numQubits(numQubits), qubitRegister(numQubits, "base", 0){
    }

    ~IntelSimulator(){
        //TODO: ensure everything is safely freed here
    }

    inline void applyGateX(CST qubitIndex){ qubitRegister.applyPauliX(qubitIndex); }
    inline void applyGateY(CST qubitIndex){ qubitRegister.applyPauliY(qubitIndex); }
    inline void applyGateZ(CST qubitIndex){ qubitRegister.applyPauliZ(qubitIndex); }
    inline void applyGateH(CST qubitIndex){ qubitRegister.applyHadamard(qubitIndex); }
    //inline void applyGateI(std::size_t qubitIndex){ qubitRegister.applyPauliX(qubitIndex); }

    // Defining gate operations
    inline void applyGate(std::array<ComplexDP>& gate){
        TODO
    }

    // Defining gate operations
    inline void applyGate(std::string gateName){
        TODO
    }

    inline const TinyMatrix& getGate(std::string gateName) const {
        TODO 
        //May be difficult to generalise
    }

    inline void applyGateSqrtX(CST qubitIndex){
        qubitRegister.applyPauliSqrtX(qubitIndex);
    };

    //Arbitrary rotation along axis; given in radians
    inline void applyGateRotX(CST qubitIndex) {
        qubitRegister.applyRotationX(qubitIndex);
    };
    inline void applyGateRotY(CST qubitIndex) {
        qubitRegister.applyRotationY(qubitIndex);
    };
    inline void applyGateRotZ(CST qubitIndex) {
        qubitRegister.applyRotationZ(qubitIndex);
    };

    //2 qubit gates
    inline void applyGateCU(CST control, CST target, TinyMatrix<ComplexDP, 2, 2, 32> & U){
        qubitRegister.applyControlled1QubitGate(control, target, U);
    }

    inline void applyGateCX(CST control, CST target){
        qubitRegister.applyCPauliX(control, target);
    }
    inline void applyGateCY(CST control, CST target){
        qubitRegister.applyCPauliY(control, target);
    }
    inline void applyGateCZ(CST control, CST target){
        qubitRegister.applyCPauliZ(control, target);
    }
    inline void applyGateCH(CST control, CST target){
        qubitRegister.applyCHadamard(control, target);
    }

    inline void applyGateCRotX(CST control, CST target, const double theta){
        qubitRegister.applyCRotationX(control, target, theta);
    }
    inline void applyGateCRotY(CST control, CST target, CST theta){
        qubitRegister.applyCRotationY(control, target, theta);
    }
    inline void applyGateCRotZ(CST control, CST target, const double theta){
        qubitRegister.applyCRotationZ(control, target, theta);
    }

    inline void applyGateSwap(CST q1, CST q2){
        qubitRegister.applySwap(q1, q2);
    }
    inline void applyGateSqrtSwap(){
        qubitRegister.applySqrtISwap
    }
    inline void applyGatePhaseShift(){
        //qubitRegister.apply
    }

    //3 qubit gates
    virtual void applyGateToffoli();
    virtual void applyGateFredkin();

    //Defining Qubit operations
    inline QubitRegisterType getQubitRegister() { return this->qubitRegister; }
    inline const QubitRegisterType& getQubitRegister() const { return this->qubitRegister; };

    virtual QubitType getQubit();
    virtual const QubitType& getQubit() const;

    inline std::size_t getNumQubits() { return numQubits; }

};
