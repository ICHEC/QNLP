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

#ifndef QNLP_SIMULATORINTERFACE_H
#define QNLP_SIMULATORINTERFACE_H

#include <iostream>

namespace QNLP{

    /**
    * @class	The abstract interface for implementing the QNLP-quantum 
    * simulator connector.
    */
    template <class QubitRegister>
    class SimulatorInterface { 
        virtual int g(); 
        virtual ~SimulatorInterface() {} 

        // Defining gate operations
        virtual void applyGate();

        //Defining Qubit operations
        virtual QubitRegister getQubitRegister();
        virtual const QubitRegister& getQubitRegister() const;

    };






















    struct A : SimulatorInterface<int> {
        // OK: declares three member virtual functions, two of them pure
        virtual int f() = 0, g() override = 0, h();
        // OK: destructor can be pure too
        ~A() = 0;
        // Error: pure-specifier on a function definition
        //virtual int b()=0 {}
    };
}
#endif