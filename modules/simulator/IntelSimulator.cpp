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

#ifdef ENABLE_MPI
    #include "mpi.h"
#endif

namespace QNLP{

class IntelSimulator : public SimulatorGeneral<IntelSimulator> {
    public:
    using TMDP = openqu::TinyMatrix<ComplexDP, 2, 2, 32>;
    using QRDP = QubitRegister<ComplexDP>;
    using CST = const std::size_t;

    IntelSimulator(int numQubits, bool useFusion=false) : SimulatorGeneral<IntelSimulator>(), 
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
        gates[2](1,0) = ComplexDP(0.,0.);       gates[2](1,1) = ComplexDP(-1.,0.);

        //Define I
        gates[3](0,0) = ComplexDP(1.,0.);       gates[3](0,1) = ComplexDP(0.,0.);
        gates[3](1,0) = ComplexDP(0.,0.);       gates[3](1,1) = ComplexDP(1.,0.);

        //Define Pauli H
        double coeff = (1./sqrt(2.));
        gates[4](0,0) = coeff*ComplexDP(1.,0.);   gates[4](0,1) = coeff*ComplexDP(1.,0.);
        gates[4](1,0) = coeff*ComplexDP(1.,0.);   gates[4](1,1) = -coeff*ComplexDP(1.,0.);


        #ifdef ENABLE_MPI
            int mpi_is_init;
            MPI_Initialized(&mpi_is_init);
            if (! mpi_is_init){ // Attempt init using Intel-QS MPI env
                int argc = 0;
                char** argv = new char*[argc];

                openqu::mpi::Environment env(argc, argv); //we do not expect to pass any params here
            }
            MPI_Initialized(&mpi_is_init);
            if (! mpi_is_init){ // If Intel-QS MPI env fails, use default init
                int argc = 0;
                char** argv = new char*[argc];
                MPI_Init(&argc, &argv);
            }
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        #endif


        // Set up random number generator for randomly collapsing qubit to 0 or 1
        //
        //  RACE CONDITION - not thread safe
        //  Currently not an issue due to only MPI master rank using the rnadom numbers,
        //  however this would be a problem if this changes 
        //
        std::mt19937 mt_(rd()); 
        std::uniform_real_distribution<double> dist_(0.0,1.0);
        mt = mt_;
        dist = dist_;
        if(useFusion == true)
            qubitRegister.TurnOnFusion();
    }
    ~IntelSimulator(){ }

    //#################################################
    //   TO IMPLEMENT
    //#################################################
    // 1 qubit
    inline void applyGateU(const TMDP& U, CST qubitIndex, std::string label="U"){      
        qubitRegister.Apply1QubitGate(qubitIndex, U);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(label, U.tostr(), qubitIndex);
        #endif
    }

    inline void applyGateI(std::size_t qubitIndex){
        applyGateU(getGateI(), qubitIndex, "I");
    }

    inline void applyGatePhaseShift(std::size_t qubit_idx, double angle){
        //Phase gate is identity with 1,1 index modulated by angle
        TMDP U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));
        //qubitRegister.Apply1QubitGate(qubit_idx, U);
        applyGateU(U, qubit_idx, "Phase:=" + std::to_string(angle));
    }

    // 2 qubit
    inline void applyGateSqrtSwap(  std::size_t qubit_idx0, std::size_t qubit_idx1){    
        std::cerr << "NOT YET IMPLEMENTED" << std::endl; 
        std::abort();
    }

    // 3 qubit
    inline void applyGateCCX(std::size_t ctrl_qubit0, std::size_t ctrl_qubit1, std::size_t target_qubit){
        this->applyGateNCU(this->getGateX(), std::vector<std::size_t> {ctrl_qubit0, ctrl_qubit1}, target_qubit);
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

    inline void applyGateX(CST qubitIndex){ 
        qubitRegister.ApplyPauliX(qubitIndex);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("X", getGateX().tostr(), qubitIndex);
        #endif
    }
    inline void applyGateY(CST qubitIndex){ 
        qubitRegister.ApplyPauliY(qubitIndex);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("Y", getGateY().tostr(), qubitIndex);
        #endif
    }
    inline void applyGateZ(CST qubitIndex){ 
        qubitRegister.ApplyPauliZ(qubitIndex);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("Z", getGateZ().tostr(), qubitIndex);
        #endif
    }
    inline void applyGateH(CST qubitIndex){ 
        qubitRegister.ApplyHadamard(qubitIndex);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall("H", getGateH().tostr(), qubitIndex);
        #endif
    }

    inline void applyGateSqrtX(CST qubitIndex){
        qubitRegister.ApplyPauliSqrtX(qubitIndex);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "\\sqrt[2]{X}", 
            matrixSqrt<decltype(getGateX())>(getGateX()).tostr(), 
            qubitIndex
        );
        #endif
    };
    inline void applyGateRotX(CST qubitIndex, double angle) {
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_X(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
        qubitRegister.ApplyRotationX(qubitIndex, angle);
    };
    inline void applyGateRotY(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationY(qubitIndex, angle);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_Y(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
    };
    inline void applyGateRotZ(CST qubitIndex, double angle) {
        qubitRegister.ApplyRotationZ(qubitIndex, angle);
        #ifdef GATE_LOGGING
        writer.oneQubitGateCall(
            "R_Z(\\theta=" + std::to_string(angle) + ")", 
            getGateI().tostr(), 
            qubitIndex
        );
        #endif
    };

    inline TMDP getGateX(){ return gates[0]; }
    inline TMDP getGateY(){ return gates[1]; }
    inline TMDP getGateZ(){ return gates[2]; }
    inline TMDP getGateI(){ return gates[3]; }
    inline TMDP getGateH(){ return gates[4]; }

    inline void applyGateCU(const TMDP& U, CST control, CST target, std::string U_label="CU"){
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( U_label, U.tostr(), control, target );
        #endif
    }
    inline void applyGateCX(CST control, CST target){
        qubitRegister.ApplyCPauliX(control, target);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "X", getGateX().tostr(), control, target );
        #endif
    }
    inline void applyGateCY(CST control, CST target){
        qubitRegister.ApplyCPauliY(control, target);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "Y", getGateY().tostr(), control, target );
        #endif
    }
    inline void applyGateCZ(CST control, CST target){
        qubitRegister.ApplyCPauliZ(control, target);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "Z", getGateZ().tostr(), control, target );
        #endif
    }
    inline void applyGateCH(CST control, CST target){
        qubitRegister.ApplyCHadamard(control, target);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "H", getGateH().tostr(), control, target );
        #endif
    }

    inline void applyGateCPhaseShift(double angle, unsigned int control, unsigned int target){
        openqu::TinyMatrix<ComplexDP, 2, 2, 32> U(gates[3]);
        U(1, 1) = ComplexDP(cos(angle), sin(angle));
        qubitRegister.ApplyControlled1QubitGate(control, target, U);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CPhase", U.tostr(), control, target );
        #endif
    }

    inline void applyGateCRotX(CST control, CST target, const double theta){
        qubitRegister.ApplyCRotationX(control, target, theta);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_X", getGateI().tostr(), control, target );
        #endif
    }
    inline void applyGateCRotY(CST control, CST target, CST theta){
        qubitRegister.ApplyCRotationY(control, target, theta);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_Y", getGateI().tostr(), control, target );
        #endif
    }
    inline void applyGateCRotZ(CST control, CST target, const double theta){
        qubitRegister.ApplyCRotationZ(control, target, theta);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "CR_Z", getGateI().tostr(), control, target );
        #endif
    }

    inline void applyGateSwap(CST q1, CST q2){
        qubitRegister.ApplySwap(q1, q2);
        #ifdef GATE_LOGGING
        writer.twoQubitGateCall( "SWAP", getGateI().tostr(), q1, q2 );
        #endif
    }

    inline QubitRegister<ComplexDP>& getQubitRegister() { 
        return this->qubitRegister; 
    }

    inline const QubitRegister<ComplexDP>& getQubitRegister() const { 
        return this->qubitRegister; 
    };

    std::size_t getNumQubits() { 
        return numQubits; 
    }

    inline void initRegister(){
        this->qubitRegister.Initialize("base",0);
    }

    inline void applyAmplitudeNorm(){
        this->qubitRegister.Normalize();
    }

    // Apply measurement to single qubit
    inline bool applyMeasurement(CST target, bool normalize=true){
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

        collapseQubit(target,(bit_val = (rand < getStateProbability(target))));
        if(normalize){
            applyAmplitudeNorm();
        }
        return bit_val;
    }

    // Measurement methods
    inline void collapseToBasisZ(CST target, bool collapseValue){
        collapseQubit(target, collapseValue);
        applyAmplitudeNorm();
    }

    // State observation methods: not allowed in quantum operations
    inline void PrintStates(std::string x, std::vector<std::size_t> qubits = {}){
        qubitRegister.Print(x,qubits);
    }

    #ifdef GATE_LOGGING
    GateWriter& getGateWriter(){
        return writer;
    } 
    #endif

    private:
    std::size_t numQubits = 0;
    QRDP qubitRegister;
    std::vector<TMDP> gates;
    #ifdef ENABLE_MPI
        int rank;
    #endif

    //  RACE CONDITION - not thread safe
    //  Currently not an issue due to only MPI master rank using the rnadom numbers,
    //  however this would be a problem if this changes 
    std::random_device rd; 
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;

    // Measurement methods
    inline void collapseQubit(CST target, bool collapseValue){
        qubitRegister.CollapseQubit(target, collapseValue);
    }

    inline double getStateProbability(CST target){
        return qubitRegister.GetProbability(target);
    }
};

// Generate templated classes/methods
//template class Oracle<IntelSimulator>;
//template void SimulatorGeneral<IntelSimulator>::applyGateNCU<IntelSimulator::TMDP>(const IntelSimulator::TMDP& U, std::size_t minIdx, std::size_t maxIdx, std::size_t target, std::string label);

//template void SimulatorGeneral<IntelSimulator>::applyGateNCU<IntelSimulator::TMDP>(const IntelSimulator::TMDP& U, const std::vector<std::size_t>& ctrlIndices, std::size_t target, std::string label);

};
