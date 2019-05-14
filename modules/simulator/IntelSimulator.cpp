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
#include <cstdlib>

namespace QNLP{

using TMDP = openqu::TinyMatrix<ComplexDP, 2, 2, 32>;
using QRDP = QubitRegister<ComplexDP>;
using CST = const std::size_t;

class IntelSimulator : public SimulatorGeneral<IntelSimulator> {
    private:
        std::size_t numQubits = 0;
        QRDP qubitRegister;
        std::vector<TMDP> gates;

    public:
    IntelSimulator(int numQubits) : SimulatorGeneral<IntelSimulator>(), 
                                    numQubits(numQubits), 
                                    qubitRegister(QubitRegister<ComplexDP> (numQubits, "base", 0)),
                                    gates(5){
        //Define Pauli X
        gates[0](0,0) = ComplexDP(0.,0.);       gates[0](0,1) = ComplexDP(1.,0.);
        gates[0](1,0) = ComplexDP(1.,0.);       gates[0](1,1) = ComplexDP(0.,0.);

        //Define Pauli Y
        gates[1](0,0) = ComplexDP(0.,0.);       gates[1](0,1) = -ComplexDP(0.,1.);
        gates[1](1,0) = ComplexDP(0.,1.);       gates[1](1,1) = ComplexDP(0.,0.);

        //Define Pauli Z
        gates[2](0,0) = ComplexDP(1.,0.);       gates[2](0,1) = ComplexDP(0.,0.);
        gates[2](1,0) = ComplexDP(0.,0.);       gates[2](1,1) = -ComplexDP(1.,0.);

        //Define I
        gates[3](0,0) = ComplexDP(1.,0.);       gates[3](0,1) = ComplexDP(0.,0.);
        gates[3](1,0) = ComplexDP(0.,0.);       gates[3](1,1) = ComplexDP(1.,0.);

        //Define Pauli H
        double coeff = (1./sqrt(2.));
        gates[4](0,0) = coeff*ComplexDP(1.,0.);   gates[4](0,1) = coeff*ComplexDP(1.,0.);
        gates[4](1,0) = coeff*ComplexDP(1.,0.);   gates[4](1,1) = -coeff*ComplexDP(1.,0.);
    }
    ~IntelSimulator(){ }

    //#################################################
    //   TO IMPLEMENT
    //#################################################
    // 1 qubit
    inline void applyGateU(const TMDP& U, CST qubitIndex){      
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort(); 
    }
    inline void applyGateI(std::size_t qubitIndex){
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }
    inline void applyGatePhaseShift(std::size_t qubit_idx, double angle){
        //Phase gate is identity with 1,1 index modulated by angle
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));
        qubitRegister.Apply1QubitGate(qubit_idx, U);
    }


    // 2 qubit
    inline void applyGateSqrtSwap(  std::size_t qubit_idx0, std::size_t qubit_idx1){    
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }

    // 3 qubit
    inline void applyGateToffoli(){
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }
    inline void applyGateFredkin(){
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }

    //#################################################

    inline void applyGateX(CST qubitIndex){ qubitRegister.ApplyPauliX(qubitIndex);      }
    inline void applyGateY(CST qubitIndex){ qubitRegister.ApplyPauliY(qubitIndex);      }
    inline void applyGateZ(CST qubitIndex){ qubitRegister.ApplyPauliZ(qubitIndex);      }
    inline void applyGateH(CST qubitIndex){ qubitRegister.ApplyHadamard(qubitIndex);    }

    inline void applyGateSqrtX(CST qubitIndex){
        qubitRegister.ApplyPauliSqrtX(qubitIndex);
    };

    inline void applyGateRotX(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationX(qubitIndex, angle);
    };
    inline void applyGateRotY(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationY(qubitIndex, angle);
    };
    inline void applyGateRotZ(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationZ(qubitIndex, angle);
    };

    inline TMDP getGateX(){ return gates[0]; }
    inline TMDP getGateY(){ return gates[1]; }
    inline TMDP getGateZ(){ return gates[2]; }
    inline TMDP getGateI(){ return gates[3]; }
    inline TMDP getGateH(){ return gates[4]; }

    //2 qubit gates
    //template<>
    //inline void applyGateCU<TMDP>(TMDP& U, CST control, CST target){
    inline void applyGateCU(const TMDP& U, CST control, CST target){
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

    inline void applyGateCPhaseShift(double angle, unsigned int control, unsigned int target){
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
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
    inline QubitRegister<ComplexDP>& getQubitRegister() { return this->qubitRegister; }
    inline const QubitRegister<ComplexDP>& getQubitRegister() const { return this->qubitRegister; };

    inline std::size_t getNumQubits() { return numQubits; }


    /**
     * Initialise register to |0....0>
     */
    inline void initRegister(){
        this->qubitRegister.Initialize("base",0);
    }

    inline void applyQFT(std::size_t minIdx, std::size_t maxIdx){
        qft.applyQFT(*this, minIdx, maxIdx);
    }
    inline void applyIQFT(std::size_t minIdx, std::size_t maxIdx){
        qft.applyIQFT(*this, minIdx, maxIdx);
    }

    private:
    NCU<TMDP, IntelSimulator> ncu;
};

};
