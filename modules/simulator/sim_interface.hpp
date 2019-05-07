//##############################################################################
/**
 *  @file    sim_interface.hpp
 *  @author  Lee J. O'Riordan
 *  @date    25/01/2019
 *  @version 0.1
 *
 *  @brief Abstract class for defining a simulator interface
 *
 *  @section DESCRIPTION
 *  This class is for implementing a mapping between the QNLP code and the 
 *  underlying quantum simulator library.
 * 
 */
//##############################################################################

#ifndef QNLP_SIMULATOR_INTERFACE_H
#define QNLP_SIMULATOR_INTERFACE_H

#include <iostream>

namespace QNLP{

    /**
    * @class	The abstract interface for implementing the QNLP-quantum 
    * simulator connector.
    */
    template <class QubitRegisterType, class QubitType>
    class Simulator {
    public:
        virtual ~Simulator() = 0;

        // Defining gate operations
        virtual void applyGate() = 0;
        virtual void getGate() = 0;

        //Single qubit gates
        virtual void applyGateU(); //Arbitrary user-defined unitary gate

        virtual void applyGateX() = 0;
        virtual void applyGateY() = 0;
        virtual void applyGateZ() = 0;
        virtual void applyGateI() = 0;
        virtual void applyGateH() = 0;

        virtual void applyGateSqrtX() = 0;

        //Arbitrary rotation along axis; given in radians
        virtual void applyGateRotX() = 0;
        virtual void applyGateRotY() = 0;
        virtual void applyGateRotZ() = 0;

        virtual void getGateX() = 0;
        virtual void getGateY() = 0;
        virtual void getGateZ() = 0;
        virtual void getGateI() = 0;
        virtual void getGateH() = 0;

        //2 qubit gates
        virtual void applyGateCU() = 0;
        virtual void applyGateCX() = 0;
        virtual void applyGateCY() = 0;
        virtual void applyGateCZ() = 0;
        virtual void applyGateCH() = 0;

        virtual void applyGateSwap() = 0;
        virtual void applyGateSqrtSwap() = 0;
        virtual void applyGatePhaseShift() = 0;

        //3 qubit gates
        virtual void applyGateToffoli() = 0;
        virtual void applyGateFredkin() = 0;

        //Defining Qubit operations
        QubitRegisterType getQubitRegister() { return this->qubitRegister; }
        const QubitRegisterType& getQubitRegister() const { return this->qubitRegister; };

        virtual QubitType getQubit();
        virtual const QubitType& getQubit() const;

        std::size_t getNumQubits() { return numQubits; }

        extern enum SimBackend;
        std::unique_ptr<Simulator> createSimulator(SimBackend s, std::size_t numQubits);

    protected:
        std::size_t numQubits = 0;
        QubitRegisterType qubitRegister;
    };

    struct A : Simulator<std::vector<int>, int> {
        // OK: declares three member virtual functions, two of them pure
        virtual int f() = 0, g() override = 0, h();
        // OK: destructor can be pure too
        ~A() = 0;
        // Error: pure-specifier on a function definition
        //virtual int b()=0 {}
    };
}
#endif