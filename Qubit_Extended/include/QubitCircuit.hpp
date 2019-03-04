#pragma once
#include "qureg/qureg.hpp"

template <class Type = ComplexDP>
class QubitCircuit: public QubitRegister<Type>{
    private:
        std::random_device rd;
        std::mt19937 mt;
        std::uniform_real_distribution<double> dist;

    public:
        QubitCircuit(std::size_t num_qubits_);

        void ApplyMeasurement(std::size_t qubit);
};

// Constructor initialising RNG
template <class Type>
QubitCircuit<Type>::QubitCircuit(std::size_t num_qubits_) : QubitRegister<Type>( num_qubits_,"base",0) {
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
