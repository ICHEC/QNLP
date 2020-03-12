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
#include "GateWriter.hpp"
#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <cstdlib>
#include <iostream>
#include <limits>

#ifdef ENABLE_MPI
    #include "mpi.h"
#endif

namespace QNLP{

/**
 * @brief Class definition for IntelSimulator. The purpose of this class is to map the functionality of the underlying quantum simulator to this class so that it can be used as the template for the CRTP templated SimulatorGeneral class.
 * 
 */
class IntelSimulator : public SimulatorGeneral<IntelSimulator> {
    public:
    using TMDP = qhipster::TinyMatrix<ComplexDP, 2, 2, 32>;
    using QRDP = QubitRegister<ComplexDP>;
    using CST = const std::size_t;

    /**
     * @brief Construct a new Intel Simulator object. The constructor also sets up and initialises the MPI environemnt if MPI is enabled in the build process.
     * 
     * @param numQubits Number of qubits in quantum register
     * @param useFusion Implement gate fusion (default is False)
     */
    IntelSimulator(int numQubits, bool useFusion=false) : SimulatorGeneral<IntelSimulator>(), 
                                    numQubits(numQubits), 
                                    qubitRegister(QubitRegister<ComplexDP> (numQubits, "base", 0)),
                                    gates(5), uid( reinterpret_cast<std::size_t>(this) ){

        //Define Pauli X
        gates[0](0,0) = ComplexDP(0.,0.);       gates[0](0,1) = ComplexDP(1.,0.);
        gates[0](1,0) = ComplexDP(1.,0.);       gates[0](1,1) = ComplexDP(0.,0.);

        //Define Pauli Y
        gates[1](0,0) = ComplexDP(0.,0.);       gates[1](0,1) = -ComplexDP(0.,1.);
        gates[1](1,0) = ComplexDP(0.,1.);       gates[1](1,1) = ComplexDP(0.,0.);

        //Define Pauli Z
        gates[2](0,0) = ComplexDP(1.,0.);       gates[2](0,1) = ComplexDP(0.,0.);
        gates[2](1,0) = ComplexDP(0.,0.);       gates[2](1,1) = ComplexDP(-1.,0.);

        //Define I
        gates[3](0,0) = ComplexDP(1.,0.);       gates[3](0,1) = ComplexDP(0.,0.);
        gates[3](1,0) = ComplexDP(0.,0.);       gates[3](1,1) = ComplexDP(1.,0.);

        //Define Pauli H
        double coeff = (1./sqrt(2.));
        gates[4](0,0) = coeff*ComplexDP(1.,0.);   gates[4](0,1) = coeff*ComplexDP(1.,0.);
        gates[4](1,0) = coeff*ComplexDP(1.,0.);   gates[4](1,1) = -coeff*ComplexDP(1.,0.);

        //Ensure the cache maps are populated before use.
        this->initCaches();

        #ifdef ENABLE_MPI //If for some strange reason the MPI environement is not enable through the Base CRTP class, enable using Intel-QS
            int mpi_is_init;
            MPI_Initialized(&mpi_is_init);
            if (! mpi_is_init){ // Attempt init using Intel-QS MPI env
                int argc_tmp = 0;
                char** argv_tmp = new char*[argc_tmp];

                qhipster::mpi::Environment env(argc_tmp, argv_tmp); //we do not expect to pass any params here
                delete argv_tmp;
            }
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        #endif

        /* Set up random number generator for randomly collapsing qubit to 0 or 1
         *
         * Note: a random number will be generated on rank 0 and then broadcasted
         * to all ranks so that each rank collapses the respective qubit to the
         * same value.
         */
        std::mt19937 mt_(rd()); 
        std::uniform_real_distribution<double> dist_(0.0,1.0);
        mt = mt_;
        dist = dist_;
        if(useFusion == true){
            qubitRegister.TurnOnFusion();
            std::cerr << "Warning: enabling fusion may cause inconsistent results." << std::endl;
        }
        gate_count_1qubit = 0;
        gate_count_2qubit = 0;
    }

    /**
     * @brief Destroy the Intel Simulator object
     * 
     */
    ~IntelSimulator(){ }

    // 1 qubit
    /**
     * @brief Apply arbitrary user-defined unitary gate to qubit at qubit_idx
     * 
     * @param U User-defined unitary 2x2 matrix of templated type Mat2x2Type
     * @param qubitIndex Index of qubit to apply gate upon
     * @param label Label for the gate U
     */
    inline void applyGateU(const TMDP& U, CST qubitIndex, std::string label="U"){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.Apply1QubitGate(qubitIndex, U);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(label, U.tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply the Identity gate to the given qubit
     * 
     * @param qubitIndex 
     */
    inline void applyGateI(std::size_t qubitIndex){
        #ifndef RESOURCE_ESTIMATE
        applyGateU(getGateI(), qubitIndex, "I");
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("I", getGateI().tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply phase shift to given Qubit; [[1 0] [0 exp(i*angle)]]
     * 
     * @param qubit_idx Qubit index to perform phase shift upon
     * @param angle Angle of phase shift in rads
     */
    inline void applyGatePhaseShift(std::size_t qubit_idx, double angle){
        //Phase gate is identity with 1,1 index modulated by angle
        TMDP U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));

        #ifndef RESOURCE_ESTIMATE
        applyGateU(U, qubit_idx, "Phase:=" + std::to_string(angle));
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("PShift(theta=" + std::to_string(angle) + ")", U.tostr(), qubit_idx);
        #endif

    }

    // 2 qubit
    /**
     * @brief Performs Sqrt SWAP gate between two given qubits (half way SWAP)
     * 
     * @param qubit_idx0 Qubit index 0
     * @param qubit_idx1 Qubit index 1
     */
    inline void applyGateSqrtSwap(  std::size_t qubit_idx0, std::size_t qubit_idx1){    
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }

    // 3 qubit
    /**
     * @brief Controlled controlled NOT (CCNOT, CCX) gate
     * 
     * @param ctrl_qubit0 Control qubit 0
     * @param ctrl_qubit1 Control qubit 1
     * @param target_qubit Target qubit
     */
    inline void applyGateCCX(std::size_t ctrl_qubit0, std::size_t ctrl_qubit1, std::size_t target_qubit){
        this->applyGateNCU(this->getGateX(), std::vector<std::size_t> {ctrl_qubit0, ctrl_qubit1}, target_qubit, "X");
    }

    /**
     * @brief Controlled SWAP gate (Controlled swap decomposition taken from arXiV:1301.3727
)
     * 
     * @param ctrl_qubit Control qubit
     * @param qubit_swap0 Swap qubit 0
     * @param qubit_swap1 Swap qubit 1
     */
    inline void applyGateCSwap(std::size_t ctrl_qubit, std::size_t qubit_swap0, std::size_t qubit_swap1){
        //V = sqrt(X)
        TMDP V;
        V(0,0) = {0.5,  0.5};
        V(0,1) = {0.5, -0.5};
        V(1,0) = {0.5, -0.5};
        V(1,1) = {0.5,  0.5};
        
        TMDP V_dag;
        V_dag(0,0) = {0.5, -0.5};
        V_dag(0,1) = {0.5,  0.5};
        V_dag(1,0) = {0.5,  0.5};
        V_dag(1,1) = {0.5, -0.5};
        
        applyGateCX(qubit_swap1, qubit_swap0);
        applyGateCU(V, qubit_swap0, qubit_swap1, "X");
        applyGateCU(V, ctrl_qubit, qubit_swap1, "X");
        
        applyGateCX(ctrl_qubit, qubit_swap0);
        applyGateCU(V_dag, qubit_swap0, qubit_swap1, "X");
        applyGateCX(qubit_swap1, qubit_swap0);
        applyGateCX(ctrl_qubit, qubit_swap0);
    }

    //#################################################

    /**
     * @brief Apply the Pauli X gate to the given qubit
     * 
     * @param qubitIndex 
     */
    inline void applyGateX(CST qubitIndex){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyPauliX(qubitIndex);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("X", getGateX().tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply the Pauli Y gate to the given qubit
     * 
     * @param qubitIndex 
     */
    inline void applyGateY(CST qubitIndex){ 
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyPauliY(qubitIndex);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("Y", getGateY().tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply the Pauli Z gate to the given qubit
     * 
     * @param qubitIndex 
     */
    inline void applyGateZ(CST qubitIndex){ 
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyPauliZ(qubitIndex);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("Z", getGateZ().tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply the Hadamard gate to the given qubit
     * 
     * @param qubitIndex 
     */
    inline void applyGateH(CST qubitIndex){ 
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyHadamard(qubitIndex);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("H", getGateH().tostr(), qubitIndex);
        #endif
    }

    /**
     * @brief Apply the Sqrt{Pauli X} gate to the given qubit
     * 
     * @param qubit_idx 
     */
   inline void applyGateSqrtX(CST qubitIndex){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyPauliSqrtX(qubitIndex);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "\\sqrt[2]{X}", 
            matrixSqrt<decltype(getGateX())>(getGateX()).tostr(), 
            qubitIndex
        );
        #endif
    };

    /**
     * @brief Apply the given Rotation about X-axis to the given qubit
     * 
     * @param qubitIndex Index of qubit to rotate about X-axis
     * @param angle Rotation angle
     */
    inline void applyGateRotX(CST qubitIndex, double angle) {
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyRotationX(qubitIndex, angle);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_X(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
    };

    /**
     * @brief Apply the given Rotation about Y-axis to the given qubit
     * 
     * @param qubitIndex Index of qubit to rotate about X-axis
     * @param angle Rotation angle
     */
    inline void applyGateRotY(CST qubitIndex, double angle) {
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyRotationY(qubitIndex, angle);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_Y(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
    };

    /**
     * @brief Apply the given Rotation about Z-axis to the given qubit
     * 
     * @param qubitIndex Index of qubit to rotate about X-axis
     * @param angle Rotation angle
     */
    inline void applyGateRotZ(CST qubitIndex, double angle) {
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyRotationZ(qubitIndex, angle);
        #endif

        gate_count_1qubit++;

        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_Z(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
    };

    /**
     * @brief Get the Pauli-X gate
     * @return TMDP return type of Pauli-X gate
     */
    inline TMDP getGateX(){ return gates[0]; }

    /**
     * @brief Get the Pauli-Y gate
     * @return TMDP return type of Pauli-Y gate
     */
    inline TMDP getGateY(){ return gates[1]; }

    /**
     * @brief Get the Pauli-Z gate
     * @return TMDP return type of Pauli-Z gate
     */
    inline TMDP getGateZ(){ return gates[2]; }

    /**
     * @brief Get the Identity
     * @return TMDP return type of the Identity
     */
    inline TMDP getGateI(){ return gates[3]; }

    /**
     * @brief Get the Hadamard gate
     * @return TMDP return type of Hadamard gate
     */
    inline TMDP getGateH(){ return gates[4]; }

    /**
     * @brief Apply the given controlled unitary gate on target qubit
     * 
     * @param U User-defined arbitrary 2x2 unitary gate (matrix)
     * @param control Qubit index acting as control
     * @param target Qubit index acting as target
     * @param label Optional parameter to label the gate U
     */
    inline void applyGateCU(const TMDP& U, CST control, CST target, std::string label="U"){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( label, U.tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply Controlled Pauli-X (CNOT) on target qubit
     * 
     * @param control Qubit index acting as control
     * @param target Qubit index acting as target
     */
    inline void applyGateCX(CST control, CST target){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCPauliX(control, target);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "X", getGateX().tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply Controlled Pauli-Y on target qubit
     * 
     * @param control Qubit index acting as control
     * @param target Qubit index acting as target
     */
    inline void applyGateCY(CST control, CST target){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCPauliY(control, target);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "Y", getGateY().tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply Controlled Pauli-Z on target qubit
     * 
     * @param control Qubit index acting as control
     * @param target Qubit index acting as target
     */
    inline void applyGateCZ(CST control, CST target){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCPauliZ(control, target);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "Z", getGateZ().tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply Controlled Hadamard on target qubit
     * 
     * @param control Qubit index acting as control
     * @param target Qubit index acting as target
     */
    inline void applyGateCH(CST control, CST target){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCHadamard(control, target);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "H", getGateH().tostr(), control, target );
        #endif
    }

    /**
     * @brief Perform controlled phase shift gate 
     * 
     * @param angle Angle of phase shift in rads
     * @param control Index of control qubit
     * @param target Index of target qubit
     */
    inline void applyGateCPhaseShift(double angle, unsigned int control, unsigned int target){
        TMDP U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));

        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CPhase", U.tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply the given Controlled Rotation about X-axis to the given qubit
     * 
     * @param control Control qubit
     * @param target Index of qubit to rotate about X-axis
     * @param theta Rotation angle
     */
    inline void applyGateCRotX(CST control, CST target, const double theta){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCRotationX(control, target, theta);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_X", getGateI().tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply the given Controlled Rotation about Y-axis to the given qubit
     * 
     * @param control Control qubit
     * @param target Index of qubit to rotate about Y-axis
     * @param theta Rotation angle
     */
    inline void applyGateCRotY(CST control, CST target, double theta){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCRotationY(control, target, theta);
        #endif

        gate_count_2qubit++;

        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_Y", getGateI().tostr(), control, target );
        #endif
    }

    /**
     * @brief Apply the given Controlled Rotation about Z-axis to the given qubit
     * 
     * @param control Control qubit
     * @param target Index of qubit to rotate about Z-axis
     * @param theta Rotation angle
     */
    inline void applyGateCRotZ(CST control, CST target, const double theta){
        #ifndef RESOURCE_ESTIMATE
        qubitRegister.ApplyCRotationZ(control, target, theta);
        #endif
        
        gate_count_2qubit++;
        
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_Z", getGateI().tostr(), control, target );
        #endif
    }

    /**
     * @brief Swap the qubits at the given indices
     * 
     * @param qubit_idx0 Index of qubit 0 to swap &(0 -> 1)
     * @param qubit_idx1 Index of qubit 1 to swap &(1 -> 0)
     */
    inline void applyGateSwap(CST qubit_idx0, CST qubit_idx1){
        qubitRegister.ApplySwap(qubit_idx0, qubit_idx1);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "SWAP", getGateI().tostr(), qubit_idx0, qubit_idx1 );
        #endif
    }

    /**
     * @brief Get the Qubit Register object
     * 
     * @return QubitRegister<ComplexDP>& Returns a refernce to the Qubit Register object 
     */
    inline QubitRegister<ComplexDP>& getQubitRegister() { 
        return this->qubitRegister; 
    }

    /**
     * @brief Get the Qubit Register object
     * 
     * @return const QubitRegister<ComplexDP>& Returns a refernce to the Qubit Register object  
     */
    inline const QubitRegister<ComplexDP>& getQubitRegister() const { 
        return this->qubitRegister; 
    };

    /**
     * @brief Get the number of Qubits
     * 
     * @return std::size_t Number of qubits in register
     */
    std::size_t getNumQubits() { 
        return numQubits; 
    }

    /**
     * @brief (Re)Initialise the underlying register of the encapsulated simulator to well-defined state (|0....0>)
     * 
     */
    void initRegister(){
        this->qubitRegister.Initialize("base",0);
        this->initCaches();
        gate_count_1qubit = 0;
        gate_count_2qubit = 0;
    }

    /**
     * @brief Apply normalization to the amplitudes of each state. This is required after a qubit in a state is collapsed.
     * 
     */
    inline void applyAmplitudeNorm(){
        this->qubitRegister.Normalize();
    }

    /**
     * @brief Apply measurement to a target qubit, randomly collapsing the qubit proportional to the amplitude and returns the collapsed value. If built with MPI enabled, this member function collpases the corresponding qubit in each state across all processes.
     * 
     * @return bool Value that qubit is randomly collapsed to
     * @param target The index of the qubit being collapsed
     * @param normalize Optional argument specifying whether amplitudes should be normalized (true) or not (false). Default value is true.
     */
    bool applyMeasurement(CST target, bool normalize=true){
        double rand;
        bool bit_val;

        #ifdef ENABLE_MPI
            if(rank == 0){
                rand = dist(mt);
            }
            MPI_Bcast(&rand, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
        #else
            rand = dist(mt);
        #endif
        collapseQubit(target, (bit_val = ( rand < getStateProbability(target) ) ) );
        if(normalize){
            applyAmplitudeNorm();
        }
        return bit_val;
    }

    /**
     * @brief Apply measurement to a target qubit with respect to the Z-basis, collapsing to a specified value (0 or 1). Amplitudes are r-normalized afterwards. 
     * 
     * @param target The index of the qubit being collapsed
     * @param collapseValue The value that the register will be collapsed to (either 0 ro 1).
     */
    void collapseToBasisZ(CST target, bool collapseValue){
        collapseQubit(target, collapseValue);
        applyAmplitudeNorm();
    }

    /**
     * @brief Prints the string x and then for each state of the specified qubits in the superposition, prints each its amplitude, followed by state and then by the probability of that state. Note that this state observation method is not a permitted quantum operation, however it is provided for convenience and debugging/testing. 
     * 
     * @param x String to be printed to stdout
     * @param qubits Indices of qubits in register to be printed
     */
    inline void PrintStates(std::string x, std::vector<std::size_t> qubits = {}){
        qubitRegister.Print(x,qubits);
    }

    #ifdef GATE_LOGGING
    /**
     * @brief Get the Gate Writer object
     * 
     * @return GateWriter& Returns reference to the writer member in the class 
     */
    GateWriter& getGateWriter(){
        return writer;
    } 
    #endif

    /**
     * @brief Print 1 and 2 qubit gate call counts.
     * 
     */
    std::pair<std::size_t, std::size_t> getGateCounts(){
        std::cout << "######### Gate counts #########" << std::endl;
        std::cout << "1 qubit = " << gate_count_1qubit << std::endl;
        std::cout << "2 qubit = " << gate_count_2qubit << std::endl;
        std::cout << "total = " << gate_count_1qubit + gate_count_2qubit << std::endl;
        std::cout << "###############################" << std::endl;
        return std::make_pair(gate_count_1qubit, gate_count_2qubit);
    }

    /**
     * @brief Compute overlap between different simulators. 
     * Number of qubits must be the same
     *
     */
    inline complex<double> overlap( IntelSimulator &sim){
        if(sim.uid != this->uid){
            return qubitRegister.ComputeOverlap(sim.qubitRegister);
        }
        else{
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    private:
    //inline static std::size_t suid = 0; //works in C++17.
    const std::size_t uid;

    std::size_t numQubits = 0;
    QRDP qubitRegister;
    std::vector<TMDP> gates;
    #ifdef ENABLE_MPI
        int rank;
    #endif

    std::size_t gate_count_1qubit;
    std::size_t gate_count_2qubit;

    std::random_device rd; 
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;


    // Measurement methods
    /**
     * @brief Collapses specified qubit in register to the collapseValue without applying normalization.
     * 
     * @param target Index of qubit to be collapsed 
     * @param collapseValue Value qubit is collapsed to (0 or 1)
     */
    inline void collapseQubit(CST target, bool collapseValue){
        qubitRegister.CollapseQubit(target, collapseValue);
    }

    /**
     * @brief Get the probability of the specified qubit being in the state |1>
     * 
     * @param target Target qubit 
     * @return double Probability that the target qubit is in the state |1>
     */
    inline double getStateProbability(CST target){
        return qubitRegister.GetProbability(target);
    }

};

};

