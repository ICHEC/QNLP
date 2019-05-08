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

#include "Simulator.hpp"
#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"

using namespace QNLP;
using TMDP = openqu::TinyMatrix<ComplexDP, 2, 2, 32>;
using QRDP = QubitRegister<ComplexDP>;
using CST = const std::size_t;

class IntelSimulator : Simulator<QRDP, TMDP> {
    //IntelSimulator(int numQubits) : Simulator<QubitRegister<ComplexDP>, TMDP>(numQubits), qubitRegister(numQubits, "base", 0){
    IntelSimulator(int numQubits){// : Simulator<QubitRegister<ComplexDP>, TMDP>(numQubits), qubitRegister(numQubits, "base", 0){
        this->numQubits = numQubits;
        this->qubitRegister = QubitRegister<ComplexDP> (numQubits, "base", 0);
    }

    ~IntelSimulator(){
        //TODO: ensure everything is safely freed here
    }

    //#################################################
    //   TO IMPLEMENT
    //#################################################
    // 1 qubit
    inline void applyGateU(TMDP& U, CST qubitIndex){            std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline void applyGateI(std::size_t qubitIndex){             std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline void applyGatePhaseShift(std::size_t qubit_idx){     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline TMDP getGateX(){                                     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline TMDP getGateY(){                                     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline TMDP getGateZ(){                                     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline TMDP getGateI(){                                     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline TMDP getGateH(){                                     std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    // 2 qubit
    inline void applyGateSqrtSwap(  std::size_t qubit_idx0, 
                                    std::size_t qubit_idx1){    std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    // 3 qubit
    inline void applyGateToffoli(){                             std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }
    inline void applyGateFredkin(){                             std::cerr << "NOT YET IMPLEMENTED" << std::endl; exit(-1); }

    //#################################################

    inline void applyGateX(CST qubitIndex){ qubitRegister.ApplyPauliX(qubitIndex); }
    inline void applyGateY(CST qubitIndex){ qubitRegister.ApplyPauliY(qubitIndex); }
    inline void applyGateZ(CST qubitIndex){ qubitRegister.ApplyPauliZ(qubitIndex); }
    inline void applyGateH(CST qubitIndex){ qubitRegister.ApplyHadamard(qubitIndex); }

    // Defining gate operations
    inline void applyGate(TMDP& gate){
        //TODO
    }

    // Defining gate operations
    inline void applyGate(std::string gateName){
        //TODO
    }

    inline const TMDP& getGate(std::string gateName) const {
        //TODO 
        //May be difficult to generalise
    }

    inline void applyGateSqrtX(CST qubitIndex){
        qubitRegister.ApplyPauliSqrtX(qubitIndex);
    };

    //Arbitrary rotation along axis; given in radians
    inline void applyGateRotX(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationX(qubitIndex, angle);
    };
    inline void applyGateRotY(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationY(qubitIndex, angle);
    };
    inline void applyGateRotZ(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationZ(qubitIndex, angle);
    };

    //2 qubit gates
    inline void applyGateCU(CST control, CST target, TMDP& U){
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
    }

    inline void applyGateCX(CST control, CST target){
        qubitRegister.ApplyCPauliX(control, target);
    }
    inline void applyGateCY(CST control, CST target){
        qubitRegister.ApplyCPauliY(control, target);
    }
    inline void applyGateCZ(CST control, CST target){
        qubitRegister.ApplyCPauliZ(control, target);
    }
    inline void applyGateCH(CST control, CST target){
        qubitRegister.ApplyCHadamard(control, target);
    }

    inline void applyGateCRotX(CST control, CST target, const double theta){
        qubitRegister.ApplyCRotationX(control, target, theta);
    }
    inline void applyGateCRotY(CST control, CST target, CST theta){
        qubitRegister.ApplyCRotationY(control, target, theta);
    }
    inline void applyGateCRotZ(CST control, CST target, const double theta){
        qubitRegister.ApplyCRotationZ(control, target, theta);
    }

    inline void applyGateSwap(CST q1, CST q2){
        qubitRegister.ApplySwap(q1, q2);
    }

    //3 qubit gates

    //Defining Qubit operations
    inline QubitRegister<ComplexDP> getQubitRegister() { return this->qubitRegister; }
    inline const QubitRegister<ComplexDP>& getQubitRegister() const { return this->qubitRegister; };

    inline std::size_t getNumQubits() { return numQubits; }

};
