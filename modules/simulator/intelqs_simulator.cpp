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
 *  Intel Quantum Simulator.
 * 
 */
//##############################################################################

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

    inline void applyGateX(std::size_t qubitIndex){
        qubitRegister.applyPauliX(qubitIndex);
    }
    inline void applyGateY(std::size_t qubitIndex){
        qubitRegister.applyPauliY(qubitIndex);
    }
    inline void applyGateZ(std::size_t qubitIndex){
        qubitRegister.applyPauliZ(qubitIndex);
    }
    inline void applyGateI(std::size_t qubitIndex){
        qubitRegister.applyPauliX(qubitIndex);
    }
    inline void applyGateH(std::size_t qubitIndex){
        qubitRegister.applyPauliY(qubitIndex);
    }

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

    inline void applyGateSqrtX(std::size_t qubitIndex){
        qubitRegister.applyPauliSqrtX(std::size_t qubitIndex);
    };

    //Arbitrary rotation along axis; given in radians
    inline void applyGateRotX(std::size_t qubitIndex) {
        qubitRegister.applyRotationX(qubitIndex);
    };
    inline void applyGateRotY(std::size_t qubitIndex) {
        qubitRegister.applyRotationY(qubitIndex);
    };
    inline void applyGateRotZ(std::size_t qubitIndex) {
        qubitRegister.applyRotationZ(qubitIndex);
    };

    //2 qubit gates
    inline void applyGateCU(std::size_t control, std::size_t target, TinyMatrix<ComplexDP, 2, 2, 32> & U){
        qubitRegister.applyControlled1QubitGate(control, target, U);
    }

    inline void applyGateCX(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliX(control, target);
    }
    inline void applyGateCY(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliY(control, target);
    }
    inline void applyGateCZ(const std::size_t control, const std::size_t target){
        qubitRegister.applyCPauliZ(control, target);
    }
    inline void applyGateCH(const std::size_t control, const std::size_t target){
        qubitRegister.applyCHadamard(control, target);
    }

    inline void applyGateCRotX(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationX(control, target, theta);
    }
    inline void applyGateCRotY(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationY(control, target, theta);
    }
    inline void applyGateCRotZ(const std::size_t control, const std::size_t target, const double theta){
        qubitRegister.applyCRotationZ(control, target, theta);
    }

    inline void applyGateSwap(std::size_t q1, std::size_t q2){
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
