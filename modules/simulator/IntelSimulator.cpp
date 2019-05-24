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

class IntelSimulator : public SimulatorGeneral<IntelSimulator> {
    public:
    using TMDP = openqu::TinyMatrix<ComplexDP, 2, 2, 32>;
    using QRDP = QubitRegister<ComplexDP>;
    using CST = const std::size_t;

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


        // Set up random number generator for randomly collapsing qubit to 0 or 1
        //
        //  RACE CONDITION - REQUIRES FIXING
        //
        std::mt19937 mt_(rd()); 
        std::uniform_real_distribution<double> dist_(0.0,1.0);
        mt = mt_;
        dist = dist_;
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
        TMDP U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));
        qubitRegister.Apply1QubitGate(qubit_idx, U);
    }

    // 2 qubit
    inline void applyGateSqrtSwap(  std::size_t qubit_idx0, std::size_t qubit_idx1){    
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }

    // 3 qubit
    inline void applyGateCCX(std::size_t ctrl_qubit0, std::size_t ctrl_qubit1, std::size_t target_qubit){
        qubitRegister.ApplyToffoli(ctrl_qubit0, ctrl_qubit1, target_qubit);
    }

    /*
     * Controlled swap decomposition taken from arXiV:1301.3727
     */
    inline void applyGateCSwap(std::size_t ctrl_qubit, std::size_t qubit_swap0, std::size_t qubit_swap1){
        //V = sqrt(X)
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> V;
        V(0,0) = {0.5,  0.5};
        V(0,1) = {0.5, -0.5};
        V(1,0) = {0.5, -0.5};
        V(1,1) = {0.5,  0.5};
        
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> V_dag;
        V_dag(0,0) = {0.5, -0.5};
        V_dag(0,1) = {0.5,  0.5};
        V_dag(1,0) = {0.5,  0.5};
        V_dag(1,1) = {0.5, -0.5};
        
        applyGateCX(qubit_swap1, qubit_swap0);
        applyGateCU(V, qubit_swap0, qubit_swap1);
        applyGateCU(V, ctrl_qubit, qubit_swap1);
        
        applyGateCX(ctrl_qubit, qubit_swap0);
        applyGateCU(V_dag, qubit_swap0, qubit_swap1);
        applyGateCX(qubit_swap1, qubit_swap0);
        applyGateCX(ctrl_qubit, qubit_swap0);
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

    inline QubitRegister<ComplexDP>& getQubitRegister() { 
        return this->qubitRegister; 
    }
    inline const QubitRegister<ComplexDP>& getQubitRegister() const { 
        return this->qubitRegister; 
    };

    constexpr std::size_t getNumQubits() { 
        return numQubits; 
    }

    inline void initRegister(){
        this->qubitRegister.Initialize("base",0);
    }

    // Measurement methods
    inline void collapseQubit(CST target, bool collapseValue){
        this->qubitRegister.CollapseQubit(target, collapseValue);
    }

    inline double getStateProbability(CST target){
        return this->qubitRegister.GetProbability(target);
    }

    inline void applyAmplitudeNorm(){
        this->qubitRegister.Normalize();
    }

    // Apply measurement to single qubit
    inline void applyMeasurement(CST target, bool normalize=true){
        this->collapseQubit(target,(dist(mt) < this->getStateProbability(target)));
        if(normalize){
            this->applyAmplitudeNorm();
        }
    }

    private:
    std::size_t numQubits = 0;
    QRDP qubitRegister;
    std::vector<TMDP> gates;

    std::random_device rd; 
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;
};

};
