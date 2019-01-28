//##############################################################################
/**
 *  @file    gate_interface.hpp
 *  @author  Lee J. O'Riordan
 *  @date    28/01/2019
 *  @version 0.1
 *
 *  @brief Abstract class for defining a quantum gate interface
 *
 *  @section DESCRIPTION
 *  This class is for implementing a mapping between the QNLP code and the 
 *  underlying quantum simulator library's gate implementation.
 * 
 */
//##############################################################################

#ifndef QNLP_GATE_INTERFACE_H
#define QNLP_GATE_INTERFACE_H

#include <iostream>

namespace QNLP{

    /**
    * @class	The abstract interface for implementing the QNLP-quantum 
    * simulator connector.
    */
    class GateInterface {
    public:
        virtual ~SimulatorInterface() {} 

        // Defining gate operations
        virtual void applyGate();
        virtual void getGate();

        //Single qubit gates
        virtual void applyGateU(); //Arbitrary user-defined unitary gate

        virtual void applyGateX();
        virtual void applyGateY();
        virtual void applyGateZ();
        virtual void applyGateI();
        virtual void applyGateH();

        virtual void applyGateSqrtX();

        //Arbitrary rotation along axis; given in radians
        virtual void applyGateRotX();
        virtual void applyGateRotY();
        virtual void applyGateRotZ();

        virtual void getGateX();
        virtual void getGateY();
        virtual void getGateZ();
        virtual void getGateI();
        virtual void getGateH();

        //2 qubit gates
        virtual void applyGateCU();
        virtual void applyGateCX();
        virtual void applyGateCY();
        virtual void applyGateCZ();
        virtual void applyGateCH();

        virtual void applyGateSwap();
        virtual void applyGateSqrtSwap();
        virtual void applyGatePhaseShift();

        //3 qubit gates
        virtual void applyGateToffoli();
        virtual void applyGateFredkin();

        //Defining Qubit operations
        QubitRegisterType getQubitRegister() { return this->qubitRegister; }
        const QubitRegisterType& getQubitRegister() const { return this->qubitRegister; };

        virtual QubitType getQubit();
        virtual const QubitType& getQubit() const;

        std::size_t getNumQubits() { return numQubits; }

    private:
        std::size_t numQubits = 0;
        QubitRegisterType qubitRegister;
    };




















    struct A : SimulatorInterface<std::vector<int>, int> {
        // OK: declares three member virtual functions, two of them pure
        virtual int f() = 0, g() override = 0, h();
        // OK: destructor can be pure too
        ~A() = 0;
        // Error: pure-specifier on a function definition
        //virtual int b()=0 {}
    };
}
#endif