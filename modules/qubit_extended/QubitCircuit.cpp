#include "QubitCircuit.hpp"

using namespace QNLP;

//#include "util/tinymatrix.hpp"

/**
 * @brief Checks if the bit'th bit of the integer byte is set
 * 
 * @param byte - integer representing a binary string
 * @param bit - the index of the bit to be checked (beginning
 * with the least significant bit)
 */
#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

// Constructor initialising RNG
template <class Type>
QubitCircuit<Type>::QubitCircuit(std::size_t num_qubits_, std::string style, std::size_t base_index): QubitRegister<Type>( num_qubits_,style,base_index) {
    std::mt19937 mt_(rd());
    std::uniform_real_distribution<double> dist_(0.0,1.0);
    mt = mt_;
    dist = dist_;
}

// Applies a typical quantum circuit measurement to qubit indexed by qubit, renormalising the states' amplitudes after
// the measured qubit has been collapsed.
template <class Type>
void QubitCircuit<Type>::ApplyMeasurement(std::size_t qubit, bool normalize){
    assert(qubit < this->NumQubits());

    // Obtain the measured qubit by collapsing the states 
    // randomly proportional to their amplitudes
    CollapseQubit(qubit,(dist(mt) < this->GetProbability(qubit)));

    if(normalize){
        // Renormalize state coefficients
        this->Normalize();
    }
}

// Applies N qubit controlled PauliX where N is the length of the input.
//      It is assumed that the ancilla registers are in the state |0>.
//      All ancilla qubits are reset to |0> afterwards.
//
//      Requires one less ancilla qubits than control qubits, passed in as n control qubits.
template <class Type>
void QubitCircuit<Type>::ApplyNCPauliX(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, int n){
    this->ApplyToffoli(input[0],input[1],ancilla[0]);

    // Apply Toffoli repeatedly and dependent on ancillary qubit set by previous iteration of
    // loop using other qubit as one of the controls so that the history of the input qubits is
    // recorded in it.
    // The controlled ancillary qubit is then reset to zero.
    for(int j = 2; j < n; j++){
        this->ApplyToffoli(input[j],ancilla[j-2],ancilla[j-1]);
    }
    

    // Apply CNOT on target qubit with the last qubit in ancillar register as control.
    this->ApplyCPauliX(ancilla[n-2],target[0]);

    // Undo operations on ancilla qubits to return them to their original states

    for(int j = n-1; j > 1; j--){
        this->ApplyToffoli(input[j],ancilla[j-2],ancilla[j-1]);
    }

    this->ApplyToffoli(input[0],input[1],ancilla[0]);
}

// Applies N qubit controlled unitary where N is the length of the input.
//      It is assumed that the ancilla registers are in the state |0>.
//      All ancilla qubits are reset to |0> afterwards.
//
//      Requires one less ancilla qubits than control qubits, passed in as n control qubits.
template <class Type>
void QubitCircuit<Type>::ApplyNCUnitary(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, openqu::TinyMatrix<Type, 2, 2, 32> U , int n){
    this->ApplyToffoli(input[0],input[1],ancilla[0]);

    // Apply Toffoli repeatedly and dependent on ancillary qubit set by previous iteration of
    // loop using other qubit as one of the controls so that the history of the input qubits is
    // recorded in it.
    // The controlled ancillary qubit is then reset to zero.
    for(int j = 2; j < n; j++){
        this->ApplyToffoli(input[j],ancilla[j-2],ancilla[j-1]);
    }
    
    this->ApplyControlled1QubitGate(ancilla[n-2],target[0], U);

    // Undo operations on ancilla qubits to return them to their original states

    for(int j = n-1; j > 1; j--){
        this->ApplyToffoli(input[j],ancilla[j-2],ancilla[j-1]);
    }

    this->ApplyToffoli(input[0],input[1],ancilla[0]);
}

/*
 *      // Won't work ing general since measurement will destroy states in a superposition
 *
 */
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
        //this->ApplyMeasurement(ancilla[a_control]);
        //if(this->GetProbability(ancilla[a_control]) == 1){
        if(this->GetClassicalValue(ancilla[a_control]) == 1){
            this->ApplyPauliX(ancilla[a_control]);
        }   
        
        // Switch ancilla bit being updated
        tmp_a = a_control;
        a_control = a_target;
        a_target = tmp_a;
    }
    

    this->ApplyCPauliX(ancilla[a_control],target[0]);

    //this->ApplyMeasurement(ancilla[a_control]);
    //if(this->GetProbability(ancilla[a_control]) == 1){
    if(this->GetClassicalValue(ancilla[a_control]) == 1){
        this->ApplyPauliX(ancilla[a_control]);
    }        
}

/*
 *      // Won't work ing general since measurement will destroy states in a superposition
 * 
 *      DEPRECATE!!
 *
 */
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
       // this->ApplyMeasurement(ancilla[a_control]);
        if(this->GetClassicalValue(ancilla[a_control]) == 1){
            this->ApplyPauliX(ancilla[a_control]);
        }   
        
        // Switch ancilla bit being updated
        tmp_a = a_control;
        a_control = a_target;
        a_target = tmp_a;
    }
    

    this->ApplyControlled1QubitGate(ancilla[a_control],target[0], U);

    //this->ApplyMeasurement(ancilla[a_control]);
    if(this->GetClassicalValue(ancilla[a_control]) == 1){
        this->ApplyPauliX(ancilla[a_control]);
    }        
}

template <class Type>
void QubitCircuit<Type>::EncodeBinInToSuperposition(vector<unsigned>& reg_memory, vector<unsigned>& reg_ancilla, vector<unsigned>& bin_patterns, unsigned len_bin_pattern){

    static bool first_instance = true;
    int m, len_reg_ancilla;
    m = bin_patterns.size();
    len_reg_ancilla = reg_ancilla.size();

    // Require length of ancilla register to have m+2 qubits
    assert(m + 1 < len_reg_ancilla);

    // Initialise matrices used only on first instance of this operator
    if(first_instance){
        cout << "Initialising Encoding" << endl;
        first_instance = false;

        // Preparation for binary encoding:
        //
        // Prepare three matrices S^1,S^2,...,S^3 that are required for the implemented
        // algorithm to encode these binary strings into a superposition state.
        //
        // Note the matrix indexing of the S vector of S^p matrices will be backwards:
        //      S[0] -> S^p
        //      S[1] -> S_{p-1}, and so on.
        //
        S.reset(new vector<openqu::TinyMatrix<Type, 2, 2, 32>>(m));
        {
            int p = m;
            double diag, off_diag;

            for(int i = 0; i < m; i++){
                off_diag = 1.0/sqrt((double)(p));
                diag = off_diag * sqrt((double)(p-1));

                (*S)[i](0,0) = {diag, 0.0};
                (*S)[i](0,1) = {off_diag, 0.0};
                (*S)[i](1,0) = {-off_diag, 0.0};
                (*S)[i](1,1) = {diag, 0.0};

                p--;
            }
        }

        // Set up operator for ncontrolled Unitary gate for the binary
        // decomposition of the chosen state.
        X.reset(new openqu::TinyMatrix<Type, 2, 2, 32> ());
        (*X)(0,0) = {0.,  0.};
        (*X)(0,1) = {1., 0.};
        (*X)(1,0) = {1., 0.};
        (*X)(1,1) = {0.,  0.};

        op_nCDecomp.reset(new NCU<ComplexDP>(*X, len_bin_pattern));
    }
        
    // Prepare state in |0...>|0...0>|10> of lengths n,n,2
    this->ApplyPauliX(reg_ancilla[len_reg_ancilla-1]);


    // Begin Encoding
    for(int i = 0; i < m; i++){
        // Psi0
        // Encode inputted binary pattern to pReg
        for(int j = 0; j < len_bin_pattern; j++){
            if(IS_SET(bin_patterns[i],j)){
                this->ApplyPauliX(reg_ancilla[j]);
            }
        }

        // Psi1
        // Encode inputted binary pattern
        for(int j = 0; j < len_bin_pattern; j++){
            this->ApplyToffoli(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
        }

        // Psi2
        for(int j = 0; j < len_bin_pattern; j++){
            this->ApplyCPauliX(reg_ancilla[j], reg_memory[j]);
            this->ApplyPauliX(reg_memory[j]);
        }


        // Psi3
        op_nCDecomp->applyNQubitControl(*this, reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2], *X, 0, true);

        // Psi4
        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        this->ApplyControlled1QubitGate(reg_ancilla[len_reg_ancilla-2], reg_ancilla[len_reg_ancilla-1], (*S)[i]);

        // Psi5
        op_nCDecomp->applyNQubitControl(*this, reg_memory[0], reg_memory[len_bin_pattern-1], reg_ancilla[len_reg_ancilla-2], *X, 0, true);

        // Psi6 
        for(int j = len_bin_pattern-1; j > -1; j--){
            this->ApplyPauliX(reg_memory[j]);
            this->ApplyCPauliX(reg_ancilla[j], reg_memory[j]);
        }

        // Psi7
        for(int j = len_bin_pattern-1; j > -1; j--){
            this->ApplyToffoli(reg_ancilla[j], reg_ancilla[len_reg_ancilla-1], reg_memory[j]);
        }


        // Reset the p register of the new term to the state |0...0>
        for(int j = 0; j < len_bin_pattern; j++){
            // Check current pattern against next pattern
            if(IS_SET(bin_patterns[i],j)){
                this->ApplyPauliX(reg_ancilla[j]);
            }

        }
    }
}

template class QubitCircuit<ComplexDP>;
//template class QubitCircuit<ComplexSP>;
