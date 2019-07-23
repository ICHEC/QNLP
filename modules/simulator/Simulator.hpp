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
#include <vector>

#include <iostream>

// Include all additional modules to be used within simulator
#include "GateWriter.hpp"
#include "ncu.hpp"
#include "oracle.hpp"
#include "qft.hpp"
#include "bin_into_superpos.hpp"
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
    
    protected:
    #ifdef GATE_LOGGING
    GateWriter writer;
    #endif

    public:
        //using Mat2x2Type = decltype(std::declval<DerivedType>().getGateX());
        virtual ~SimulatorGeneral(){ }

        DerivedType* createSimulator(std::size_t num_qubits){
            return new DerivedType(static_cast<DerivedType&>(*this));
        }
        //##############################################################################
        //                           Single qubit gates
        //##############################################################################

        /**
         * @brief Apply the Pauli X gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateX(std::size_t qubit_idx){ 
            //#ifdef GATE_LOGGING
            writer.oneQubitGateCall("X", static_cast<DerivedType&>(*this).getGateX().tostr(), qubit_idx);
            //#endif
            static_cast<DerivedType&>(*this).applyGateX(qubit_idx);
        };
        /**
         * @brief Apply the Pauli Y gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateY(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateY(qubit_idx);
        }
        /**
         * @brief Apply the Pauli Z gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateZ(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateZ(qubit_idx);
        }
        /**
         * @brief Apply the Identity gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateI(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateI(qubit_idx); 
        }
        /**
         * @brief Apply the Hadamard gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateH(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateH(qubit_idx);
        }
        /**
         * @brief Apply the Sqrt{Pauli X} gate to the given qubit
         * 
         * @param qubit_idx 
         */
        void applyGateSqrtX(std::size_t qubit_idx){
            static_cast<DerivedType&>(*this).applyGateSqrtX(qubit_idx);
        }
        /**
         * @brief Apply the given Rotation about X-axis to the given qubit
         * 
         * @param qubit_idx Index of qubit to rotate about X-axis
         * @param angle_rad Rotation angle
         */
        void applyGateRotX(std::size_t qubit_idx, double angle_rad){
            static_cast<DerivedType&>(*this).applyGateRotX(qubit_idx, angle_rad);
        }
        /**
         * @brief Apply the given Rotation about Y-axis to the given qubit
         * 
         * @param qubit_idx Index of qubit to rotate about Y-axis
         * @param angle_rad Rotation angle
         */
        void applyGateRotY(std::size_t qubit_idx, double angle_rad){
            static_cast<DerivedType&>(*this).applyGateRotY(qubit_idx, angle_rad);
        }
        /**
         * @brief Apply the given Rotation about Z-axis to the given qubit
         * 
         * @param qubit_idx Index of qubit to rotate about Z-axis
         * @param angle_rad Rotation angle
         */
        void applyGateRotZ(std::size_t qubit_idx, double angle_rad){
            #ifdef GATE_LOGGING
            const std::string label {"R_Z(\\theta=" + std::to_string(angle_rad) + ")"};
            writer.oneQubitGateCall( label, static_cast<DerivedType&>(*this).getGateI().tostr(), qubit_idx );
            #endif
            static_cast<DerivedType&>(*this).applyGateRotZ(qubit_idx, angle_rad);
        }

        /**
         * @brief Apply arbitrary user-defined unitary gate to qubit at qubit_idx
         * 
         * @param U User-defined unitary 2x2 matrix of templated type Mat2x2Type
         * @param qubit_idx Index of qubit to apply gate upon
         */
        template<class Mat2x2Type>
        void applyGateU(const Mat2x2Type &U, std::size_t qubit_idx){
            static_cast<DerivedType*>(this)->applyGateU(U, qubit_idx);
        }

        /**
         * @brief Get the Pauli-X gate; returns templated type GateType
         * @return GateType Templated return type of Pauli-X gate
         */
        decltype(auto) getGateX() {
            return static_cast<DerivedType&>(*this).getGateX();
        }

        /**
         * @brief Get the Pauli-Y gate; must be overloaded with appropriate return type
         * @return GateType Templated return type of Pauli-Y gate
         */
        decltype(auto) getGateY(){
            return static_cast<DerivedType&>(*this).getGateY();
        }
        /**
         * @brief Get the Pauli-Z gate; must be overloaded with appropriate return type
         * @return GateType Templated return type of Pauli-Z gate
         */
        decltype(auto) getGateZ(){
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
        decltype(auto) getGateH(){
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
        void applyGateCU(const Mat2x2Type &U, std::size_t control, std::size_t target, std::string label="CU"){
            static_cast<DerivedType*>(this)->applyGateCU(U, control, target, label);
        }

        /**
         * @brief Swap the qubits at the given indices
         * 
         * @param qubit_idx0 Index of qubit 0 to swap &(0 -> 1)
         * @param qubit_idx1 Index of qubit 1 to swap &(1 -> 0)
         */
        void applyGateSwap(std::size_t qubit_idx0, std::size_t qubit_idx1){
            static_cast<DerivedType*>(this)->applyGateSwap(qubit_idx0,qubit_idx1);
        }

        /**
         * @brief Performs Sqrt SWAP gate between two given qubits (half way SWAP)
         * 
         * @param qubit_idx0 Qubit index 0
         * @param qubit_idx1 Qubit index 1
         */
        void applyGateSqrtSwap(std::size_t qubit_idx0, std::size_t qubit_idx1){
            static_cast<DerivedType*>(this)->applyGateSqrtSwap(qubit_idx0,qubit_idx1);
        }
        
        /**
         * @brief Apply phase shift to given Qubit; [[1 0] [0 exp(i*angle)]]
         * 
         * @param angle Angle of phase shift in rads
         * @param qubit_idx Qubit index to perform phase shift upon
         */
        void applyGatePhaseShift(double angle, std::size_t qubit_idx){
            static_cast<DerivedType*>(this)->applyGatePhaseShift(angle, qubit_idx);
        }

        /**
         * @brief Perform controlled phase shift gate 
         * 
         * @param angle Angle of phase shift in rads
         * @param control Index of control qubit
         * @param target Index of target qubit
         */
        void applyGateCPhaseShift(double angle, std::size_t control, std::size_t target){
            static_cast<DerivedType*>(this)->applyGateCPhaseShift(angle, control, target);
        }

        /**
         * @brief Controlled controlled NOT (CCNOT, CCX) gate
         * 
         * @param ctrl_qubit0 Control qubit 0
         * @param ctrl_qubit1 Control qubit 1
         * @param target_qubit Target qubit
         */
        void applyGateCCX(std::size_t ctrl_qubit0, std::size_t ctrl_qubit1, std::size_t target_qubit){
            static_cast<DerivedType*>(this)->applyGateCCX(ctrl_qubit0, ctrl_qubit1, target_qubit);
        }

        /**
         * @brief Controlled SWAP gate
         * 
         * @param ctrl_qubit Control qubit
         * @param qubit_swap0 Swap qubit 0
         * @param qubit_swap1 Swap qubit 1
         */
        void applyGateCSwap(std::size_t ctrl_qubit, std::size_t qubit_swap0, std::size_t qubit_swap1){
            assert( static_cast<DerivedType*>(this)->getNumQubits() > 2 );
            //The requested qubit indices must be available to use within the register range
            assert( (ctrl_qubit < getNumQubits() ) && (qubit_swap0 < getNumQubits() ) && (qubit_swap1 < getNumQubits()) );
            //The qubits must be different from one another
            assert( ctrl_qubit != qubit_swap0 && ctrl_qubit != qubit_swap1 && qubit_swap0 != qubit_swap1 );
            static_cast<DerivedType*>(this)->applyGateCSwap(ctrl_qubit, qubit_swap0, qubit_swap1);
        }

        /**
         * @brief Get the underlying qubit register object
         * 
         * @return decltype(auto) the underlying bound qubit register
         */
        decltype(auto) getQubitRegister(){ 
           return static_cast<DerivedType*>(this)->getQubitRegister();
        }

        /**
         * @brief Get the number of Qubits
         * 
         * @return std::size_t Number of qubits in register
         */
        std::size_t getNumQubits(){
            return static_cast<DerivedType*>(this)->getNumQubits();    
        }

        /**
         * @brief Apply the forward Quantum Fourier transform (QFT) to the given register index range
         * 
         * @param minIdx Lowest qubit index of the QFT range
         * @param maxIdx Highest qubit index of the QFT range
         */
        void applyQFT(std::size_t minIdx, std::size_t maxIdx){
            QFT<decltype(static_cast<DerivedType&>(*this))>::applyQFT(static_cast<DerivedType&>(*this), minIdx, maxIdx);
        }

        /**
         * @brief Apply the inverse Quantum Fourier transform (IQFT) to the given register index range
         * 
         * @param minIdx Lowest qubit index of the IQFT range
         * @param maxIdx Highest qubit index of the IQFT range
         */
        void applyIQFT(std::size_t minIdx, std::size_t maxIdx){
            QFT<decltype(static_cast<DerivedType&>(*this))>::applyIQFT(static_cast<DerivedType&>(*this), minIdx, maxIdx);
        }

        /**
         * @brief Apply n-control unitary gate to the given qubit target
         * 
         * @tparam Mat2x2Type 2x2 Matrix type of unitary gate in the format expected by the derived simulator object; decltype(simulator.getGateX()) can be used in template
         * @param U 2x2 unitary matrix
         * @param minIdx Lowest index of the control lines expected for nCU
         * @param maxIdx Highest index of the control lines expected for the nCU
         * @param target Target qubit index to apply nCU
         */
        template<class Mat2x2Type>
        void applyGateNCU(const Mat2x2Type& U, std::size_t minIdx, std::size_t maxIdx, std::size_t target){
            NCU<DerivedType> n;
            //NCU<decltype(static_cast<DerivedType&>(*this))> n;
            std::string matrixLabel = "";
            if ( U == static_cast<DerivedType*>(this)->getGateX() ){
                matrixLabel = "X";
            }
            else
                matrixLabel = "U";
            n.applyNQubitControl(static_cast<DerivedType&>(*this), minIdx, maxIdx, target, std::make_pair(matrixLabel,U), 0);
        }

        /**
         * @brief Apply oracle to match given binary index
         * 
         * @param U 2x2 unitary matrix to apply
         * @param minIdx Lowest index of the control lines expected for oracle
         * @param maxIdx Highest index of the control lines expected for oracle
         * @param target Target qubit index to apply U on
         */
        template<class Mat2x2Type>
        void applyOracle(std::size_t bit_pattern, std::size_t minIdx, std::size_t maxIdx, std::size_t target, const Mat2x2Type& U ){
            std::vector<std::size_t> control_indices;
            for(std::size_t i = minIdx; i <= maxIdx; i++){
                control_indices.push_back( i );
            }
            Oracle<DerivedType> oracle(static_cast<DerivedType*>(this)->getNumQubits()-1, control_indices);
            oracle.bitStringNCU(static_cast<DerivedType&>(*this), bit_pattern, control_indices, target, U);
        }

        /**
         * @brief Encode inputted binary strings to the memory register specified, as a superposition of states.
         * 
         * @tparam Mat2x2Type 2x2 Matrix type of unitary gate in the format expected by the derived simulator object
         * @param reg_memory std::vector of unsigned integers containing the indices of the circuit's memory register
         * @param reg_ancilla std::vector of unsigned integers type containing the indices of the circuit's ancilla register
         * @param bin_patterns std::vector of unsigned integers representing the binary patterns to encode
         * @param len_bin_pattern The length of the binary patterns being encoded
         */
        //template<class Mat2x2Type>
        void encodeBinToSuperpos(const std::vector<std::size_t>& reg_memory,
                const std::vector<std::size_t>& reg_ancilla,
                const std::vector<std::size_t>& bin_patterns,
                const std::size_t len_bin_pattern){
            EncodeBinIntoSuperpos<DerivedType> encoder(bin_patterns.size(), len_bin_pattern);
            encoder.encodeBinInToSuperpos(static_cast<DerivedType&>(*this), reg_memory, reg_ancilla, bin_patterns);
        }

        /**
         * @brief Apply measurement to a target qubit, randomly collapsing the qubit proportional to the amplitude and returns the collapsed value.
         * 
         * @return bool Value that qubit is randomly collapsed to according to amplitude
         * @param target The index of the qubit being collapsed
         * @param normalize Optional argument specifying whether amplitudes shoud be normalized (true) or not (false). Default value is true.
         */
        bool applyMeasurement(std::size_t target, bool normalize=true){
            return static_cast<DerivedType*>(this)->applyMeasurement(target, normalize);
        }

        /**
         * @brief Apply measurement to a set of target qubits, randomly collapsing the qubits proportional to the amplitude and returns the bit string of the qubits in the order they are represented in the vector of indexes, in the form of an unsigned integer.
         * 
         * @return std::size_t Integer representing the binary string of the collapsed qubits, ordered by least significant digit corresponding to first qubit in target vector of indices
         * @param target_qubits Vector of indices of qubits being collapsed
         * @param normalize Optional argument specifying whether amplitudes shoud be normalized (true) or not (false). Default value is true.
         */
        std::size_t applyMeasurementToRegister(std::vector<std::size_t> target_qubits, bool normalize=true){
            // Store current state of training register in it's integer format
            std::size_t val = 0; 
            for(int j = target_qubits.size() - 1; j > -1; j--){
                val |= (static_cast<DerivedType*>(this)->applyMeasurement(target_qubits[j], normalize) << j);
            } 
            return val;
        }
                
        /**
         * @brief (Re)Initialise the underlying register of the encapsulated simulator to well-defined state (|0....0>)
         * 
         */
        void initRegister(){
            static_cast<DerivedType*>(this)->initRegister(); 
        }

        /**
         * @brief Calculates the unitary matrix square root (U == VV, where V is returned)
         * 
         * @tparam Type ComplexDP or ComplexSP
         * @param U Unitary matrix to be rooted
         * @return openqu::TinyMatrix<Type, 2, 2, 32> V such that VV == U
         */
        template<class Mat2x2Type>
        Mat2x2Type matrixSqrt(const Mat2x2Type& U){
            Mat2x2Type V(U);
            std::complex<double> delta = U(0,0)*U(1,1) - U(0,1)*U(1,0);
            std::complex<double> tau = U(0,0) + U(1,1);
            std::complex<double> s = sqrt(delta);
            std::complex<double> t = sqrt(tau + 2.0*s);

            //must be a way to vectorise these; TinyMatrix have a scale/shift option?
            V(0,0) += s;
            V(1,1) += s;
            std::complex<double> scale_factor(1.,0.);
            scale_factor/=t;
            V(0,0) *= scale_factor; //(std::complex<double>(1.,0.)/t);
            V(0,1) *= scale_factor; //(1/t);
            V(1,0) *= scale_factor; //(1/t);
            V(1,1) *= scale_factor; //(1/t);

            return V;
        }

        /**
         * @brief Function to calculate the adjoint of an input matrix
         * 
         * @tparam Type ComplexDP or ComplexSP
         * @param U Unitary matrix to be adjointed
         * @return openqu::TinyMatrix<Type, 2, 2, 32> U^{\dagger}
         */
        template<class Mat2x2Type>
        Mat2x2Type adjointMatrix(const Mat2x2Type& U){
            Mat2x2Type Uadjoint(U);
            std::complex<double> tmp;
            tmp = Uadjoint(0,1);
            Uadjoint(0,1) = Uadjoint(1,0);
            Uadjoint(1,0) = tmp;
            Uadjoint(0,0) = std::conj(Uadjoint(0,0));
            Uadjoint(0,1) = std::conj(Uadjoint(0,1));
            Uadjoint(1,0) = std::conj(Uadjoint(1,0));
            Uadjoint(1,1) = std::conj(Uadjoint(1,1));
            return Uadjoint;
        }
    };
}
#endif
