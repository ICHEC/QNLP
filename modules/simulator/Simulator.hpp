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
#include <cstddef>
#include <utility> //std::declval

// Include all additional modules to be used within simulator
#include "ncu.hpp"
#include "qft.hpp"
//#include "arithmetic.hpp"

namespace QNLP{

#ifdef VIRTUAL_INTERFACE
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
         * @brief Arbitrary rotation around X axis by angle 'angle_rad' in radians
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

        virtual void applyGatePhaseShift(double angle, std::size_t qubit_idx) = 0;

        virtual void applyGateCPhaseShift(double angle, std::size_t control, std::size_t target) = 0;

        virtual std::size_t getNumQubits() = 0;

        //virtual void applyGateCU(const std::array<complex<double>,4>& mat2x2, std::size_t control, std::size_t target);
    };
#endif

    //##############################################################################
    //##############################################################################


    /**
     * @brief CRTP defined class for simulator implementations. 
     * 
     * @tparam DerivedType CRTP derived class simulator type
     */
    template <class DerivedType>//<class QubitRegisterType, class GateType>
#ifdef VIRTUAL_INTERFACE
    class SimulatorGeneral : virtual public ISimulator {
#else
    class SimulatorGeneral {
#endif
    public:

        virtual ~SimulatorGeneral(){ }

        DerivedType* createSimulator(std::size_t num_qubits){
            return new DerivedType(static_cast<DerivedType&>(*this));
        }
        //##############################################################################
        //                           Single qubit gates
        //##############################################################################

        void applyGateX(std::size_t qubit_idx){ 
            static_cast<DerivedType&>(*this).getNumQubits();//applyGateX(qubit_idx); 
        };
        void applyGateY(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateY(qubit_idx);
        }
        void applyGateZ(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateZ(qubit_idx);
        }
        void applyGateI(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateI(qubit_idx); 
        }
        void applyGateH(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateH(qubit_idx); 
        }
        void applyGateSqrtX(std::size_t qubit_idx){}
        void applyGateRotX(std::size_t qubit_idx, double angle_rad){}
        void applyGateRotY(std::size_t qubit_idx, double angle_rad){}
        void applyGateRotZ(std::size_t qubit_idx, double angle_rad){}

        /**
         * @brief Apply arbitrary user-defined unitary gate to qubit at qubit_idx
         * 
         * @param U User-defined unitary 2x2 matrix of templated type GateType
         * @param qubit_idx Index of qubit to apply gate upon
         */
        //virtual void applyGateU(const decltype(std::declval<DerivedType&>().getGateX()) &U, std::size_t qubit_idx) = 0;

        /**
         * @brief Get the Pauli-X gate; returns templated type GateType

         * 
         * @return GateType Templated return type of Pauli-X gate
         */
        decltype(auto) getGateX() {//-> decltype( dynamic_cast<DerivedType>(*this).getGateX() ){
            return static_cast<DerivedType&>(*this).getGateX();
        }

        /**
         * @brief Get the Pauli-Y gate; must be overloaded with appropriate return type
         * @return GateType Templated return type of Pauli-Y gate
         */
        decltype(auto) getGateY(){ //-> decltype(std::declval<DerivedType&>().getGateY());
            return static_cast<DerivedType&>(*this).getGateY();
        }
        /**
         * @brief Get the Pauli-Z gate; must be overloaded with appropriate return type
         * @return GateType Templated return type of Pauli-Z gate
         */
        decltype(auto) getGateZ(){ //-> decltype(std::declval<DerivedType&>().getGateZ());
            return static_cast<DerivedType&>(*this).getGateZ();
        }
        /**
         * @brief Get the Identity; must be overloaded with appropriate return type
         * @return GateType Templated return type of Identity gate
         */
        decltype(auto) getGateI(){
            return static_cast<DerivedType*>(this)->getGateI();
        }
        /**
         * @brief Get the Hadamard gate; must be overloaded with appropriate return type
         * @return GateType Templated return type of Hadamard gate
         */
        decltype(auto) getGateH(){// -> decltype(std::declval<DerivedType&>().getGateH());
            return static_cast<DerivedType*>(this)->getGateH();
        }
        //##############################################################################
        //                                  2 qubit gates
        //##############################################################################

        /**
         * @brief Apply the given controlled unitary gate on target qubit
         * 
         * @param U User-defined arbitrary 2x2 unitary gate (matrix)
         * @param control Qubit index acting as control
         * @param target Qubit index acting as target
         */
        template<class Mat2x2Type>
        void applyGateCU(const Mat2x2Type &U, std::size_t control, std::size_t target){
            static_cast<DerivedType*>(this)->applyGateCU(U, control, target);
        }

        /**
         * @brief Swap the qubits at the given indices
         * 
         * @param qubit_idx0 Index of qubit 0 to swap &(0 -> 1)
         * @param qubit_idx1 Index of qubit 1 to swap &(1 -> 0)
         */
        void applyGateSwap(std::size_t qubit_idx0, std::size_t qubit_idx1);

        void applyGateSqrtSwap(std::size_t qubit_idx0, std::size_t qubit_idx1);
        
        void applyGatePhaseShift(double angle, std::size_t qubit_idx){
            static_cast<DerivedType*>(this)->applyGatePhaseShift(angle, qubit_idx);
        }
        void applyGateCPhaseShift(double angle, std::size_t control, std::size_t target){
            static_cast<DerivedType*>(this)->applyGateCPhaseShift(angle, control, target);
        }

        //3 qubit gates
        void applyGateToffoli();
        void applyGateFredkin();

        //Defining Qubit operations
        //virtual decltype(auto)& getQubitRegister() -> decltype(DerivedType::getQubitRegister()) & = 0;
        decltype(auto) getQubitRegister(){ //-> decltype(std::declval<DerivedType&>().getQubitRegister())=0;
           return static_cast<DerivedType*>(this)->getQubitRegister();
        }
        //virtual auto getQubitRegister() -> decltype(DerivedType::getQubitRegister()) & const = 0;

        std::size_t getNumQubits(){
            return static_cast<DerivedType*>(this)->getNumQubits();    
        }

        void applyQFT(std::size_t minIdx, std::size_t maxIdx){
            static_cast<DerivedType&>(*this)->applyQFT(minIdx, maxIdx);
            //qft.applyQFT(static_cast<DerivedType&>(*this), minIdx, maxIdx);
        }

        void applyIQFT(std::size_t minIdx, std::size_t maxIdx){
            static_cast<DerivedType&>(*this)->applyIQFT(minIdx, maxIdx);
            //qft.applyIQFT(static_cast<DerivedType&>(*this), minIdx, maxIdx);
        }

        protected:
        QFT<DerivedType> qft;
    };
}
#endif
