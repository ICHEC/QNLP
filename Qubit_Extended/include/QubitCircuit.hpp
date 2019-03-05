#pragma once
#include "qureg/qureg.hpp"

template <class Type = ComplexDP>
class QubitCircuit: public QubitRegister<Type>{
    private:
        std::random_device rd;
        std::mt19937 mt;
        std::uniform_real_distribution<double> dist;

    public:
        QubitCircuit(std::size_t num_qubits_, std::string style = "", std::size_t base_index = 0);

        void ApplyMeasurement(std::size_t qubit);
        //void ApplyNCPauliX(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target);             // Won't work ing general since measurement will destroy states in a superposition
        //void ApplyNCUnitary(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, openqu::TinyMatrix<Type, 2, 2, 32> U);      // Won't work ing general since measurement will destroy states in a superposition
};

// Constructor initialising RNG
template <class Type>
QubitCircuit<Type>::QubitCircuit(std::size_t num_qubits_, std::string style, std::size_t base_index) : QubitRegister<Type>( num_qubits_,style,base_index) {
    std::mt19937 mt_(rd());
    std::uniform_real_distribution<double> dist_(0.0,1.0);
    mt = mt_;
    dist = dist_;
}

// Applies a typical quantum circuit measurement to qubit indexed by qubit, renormalising the states' amplitudes after
// the measured qubit has been collapsed.
template <class Type>
void QubitCircuit<Type>::ApplyMeasurement(std::size_t qubit){
        assert(qubit < this->NumQubits());

        // Obtain the measured qubit by collapsing the states 
        // randomly proportional to their amplitudes
        CollapseQubit(qubit,(dist(mt) < this->GetProbability(qubit)));

        // Renormalize state coefficients
        this->Normalize();
}

/*
 *      // Won't work ing general since measurement will destroy states in a superposition
 *
// Applies N qubit controlled PauliX where N is the length of the input.
//      It is assumed that the ancilla registers are in the state |00>.
//
//      Requires two ancilla qubits.
template <class Type>
void QubitCircuit<Type>::ApplyNCPauliX(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target){
    this->ApplyToffoli(input[0],input[1],ancilla[0]);

    int len_input = this->NumQubits() - 3; 

    // Apply Toffoli repeatedly and dependent on ancillary qubit set by previous iteration of
    // loop using other qubit as one of the controls so that the history of the input qubits is
    // recorded in it.
    // The controlled ancillary qubit is then reset to zero.
    int a_control, a_target, tmp_a;
    a_control = 0; a_target = 1;
    for(int j = 2; j < len_input; j++){
        this->ApplyToffoli(input[j],ancilla[a_control],ancilla[a_target]);


        // Reset the previously used ancilla bit to |0>.
        //          Could use len_input extra qubits to avoid
        //          conducting a measurement here. Due to the memory
        //          boundedness of the problem, a measurement was deemed 
        //          a better alternative. This is subject to change.
        this->ApplyMeasurement(ancilla[a_control]);
        if(this->GetProbability(ancilla[a_control]) == 1){
            this->ApplyPauliX(ancilla[a_control]);
        }   
        
        // Switch ancilla bit being updated
        tmp_a = a_control;
        a_control = a_target;
        a_target = tmp_a;
    }
    

    this->ApplyCPauliX(ancilla[a_control],target[0]);

    this->ApplyMeasurement(ancilla[a_control]);
    if(this->GetProbability(ancilla[a_control]) == 1){
        this->ApplyPauliX(ancilla[a_control]);
    }        
}


// Applies N qubit controlled PauliX where N is the length of the input.
//      It is assumed that the ancilla registers are in the state |00>.
//
//      Requires two ancilla qubits.
template <class Type>
void QubitCircuit<Type>::ApplyNCUnitary(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, openqu::TinyMatrix<Type, 2, 2, 32> U ){
    this->ApplyToffoli(input[0],input[1],ancilla[0]);

    int len_input = this->NumQubits() - 3; 

    // Apply Toffoli repeatedly and dependent on ancillary qubit set by previous iteration of
    // loop using other qubit as one of the controls so that the history of the input qubits is
    // recorded in it.
    // The controlled ancillary qubit is then reset to zero.
    int a_control, a_target, tmp_a;
    a_control = 0; a_target = 1;
    for(int j = 2; j < len_input; j++){
        this->ApplyToffoli(input[j],ancilla[a_control],ancilla[a_target]);


        // Reset the previously used ancilla bit to |0>.
        //          Could use len_input extra qubits to avoid
        //          conducting a measurement here. Due to the memory
        //          boundedness of the problem, a measurement was deemed 
        //          a better alternative. This is subject to change.
        this->ApplyMeasurement(ancilla[a_control]);
        if(this->GetProbability(ancilla[a_control]) == 1){
            this->ApplyPauliX(ancilla[a_control]);
        }   
        
        // Switch ancilla bit being updated
        tmp_a = a_control;
        a_control = a_target;
        a_target = tmp_a;
    }
    

    this->ApplyControlled1QubitGate(ancilla[a_control],target[0], U);

    this->ApplyMeasurement(ancilla[a_control]);
    if(this->GetProbability(ancilla[a_control]) == 1){
        this->ApplyPauliX(ancilla[a_control]);
    }        
}
*/
