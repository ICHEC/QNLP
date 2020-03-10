/**
 * @file ISimulator.hpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Define thee abstract interface class for implementing the QNLP-quantum simulator connector.
 * @version 0.1
 * @date 2020-01-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef QNLP_SIMULATOR_INTERFACE_H
#define QNLP_SIMULATOR_INTERFACE_H

#include <cstdlib>
namespace QNLP{
    /**
    * @class	The abstract interface for implementing the QNLP-quantum 
    * simulator connector. Currently exists for the purpose of later implementing 
    * dynamic polymorphism.
    */
    class ISimulator{
    public:
        //##############################################################################
        //                                  1 qubit gates
        //##############################################################################

        /**
         * @brief Apply Pauli-X gate to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateX(std::size_t qubit_idx) = 0;
        
        /**
         * @brief Apply Pauli-Y gate to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateY(std::size_t qubit_idx) = 0;
        
        /**
         * @brief Apply Pauli-Z gate to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateZ(std::size_t qubit_idx) = 0;
        
        /**
         * @brief Apply Identity to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateI(std::size_t qubit_idx) = 0;
        
        /**
         * @brief Apply Hadamard gate to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateH(std::size_t qubit_idx) = 0;

        /**
         * @brief Apply \sqrt{Pauli-X} gate to qubit at qubit_idx
         * 
         * @param qubit_idx Index of qubit to apply gate upon
         */
        virtual void applyGateSqrtX(std::size_t qubit_idx) = 0;

        /**
         * @brief Arbitrary rotation around X axis by angle 'angle_rad' in radians
         * 
         * @param qubit_idx Index of qubit to apply rotation upon
         * @param angle_rad Angle of rotation in radians
         */
        virtual void applyGateRotX(std::size_t qubit_idx, double angle_rad) = 0;

        /**
         * @brief Arbitrary rotation around Y axis by angle 'angle_rad' in radians
         * 
         * @param qubit_idx Index of qubit to apply rotation upon
         * @param angle_rad Angle of rotation in radians
         */
        virtual void applyGateRotY(std::size_t qubit_idx, double angle_rad) = 0;

        /**
         * @brief Arbitrary rotation around Z axis by angle 'angle_rad' in radians
         * 
         * @param qubit_idx Index of qubit to apply rotation upon
         * @param angle_rad Angle of rotation in radians
         */
        virtual void applyGateRotZ(std::size_t qubit_idx, double angle_rad) = 0;

        //##############################################################################
        //                                  2 qubit gates
        //##############################################################################

        /**
         * @brief Apply Controlled Pauli-X (CNOT) on target qubit
         * 
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */
        virtual void applyGateCX(std::size_t control, std::size_t target) = 0;

        /**
         * @brief Apply Controlled Pauli-Y on target qubit
         * 
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */
        virtual void applyGateCY(std::size_t control, std::size_t target) = 0;

        /**
         * @brief Apply Controlled Pauli-Z on target qubit
         * 
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */        
        virtual void applyGateCZ(std::size_t control, std::size_t target) = 0;

        /**
         * @brief Apply Controlled Hadamard on target qubit
         * 
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */
        virtual void applyGateCH(std::size_t control, std::size_t target) = 0;

        /**
         * @brief Apply Phase Shift on specified qubit by angle in radians
         * 
         * @param angle Angle of phase shift in rads
         * @param qubit_idx Index of target qubit to perform phase shift upon
         */
        virtual void applyGatePhaseShift(double angle, std::size_t qubit_idx) = 0;

        /**
         * @brief Apply Controlled Phase Shift on specified qubit by angle in radians
         * 
         * @param angle Angle of phase shift in rads
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */
        virtual void applyGateCPhaseShift(double angle, std::size_t control, std::size_t target) = 0;

        /**
         * @brief Get the Number of qubits in the simulator
         * 
         * @return std::size_t Returns the length of (number of qubits in) the quantum register.
         */
        virtual std::size_t getNumQubits() = 0;

        //virtual void applyGateCU(const std::array<complex<double>,4>& mat2x2, std::size_t control, std::size_t target);
    };
}
#endif