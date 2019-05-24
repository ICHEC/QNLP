#pragma once
//#include "qureg/qureg.hpp"
//#include "ncu.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include <memory>
//#include "util/tinymatrix.hpp"

namespace QNLP{

/**
 * @brief Class derived from QubitRegister. 
 *      Allows for N qubit controlled unitary operations and for random collapsing of qubits.
 *      
 * @tparam ComplexDP or ComplexSP or double or single precision qubit amplitudes respectively 
 */
template <class Type = >
class QubitCircuit: public QubitRegister<Type>{
    private:
        std::random_device rd;
        std::mt19937 mt;
        std::uniform_real_distribution<double> dist;

        unique_ptr< vector<openqu::TinyMatrix<Type, 2, 2, 32>>> S;
        unique_ptr<openqu::TinyMatrix<Type, 2, 2, 32>> X;
        unique_ptr<NCU<Type>> op_nCDecomp;

 
    public:
        QubitCircuit(std::size_t num_qubits_, std::string style = "", std::size_t base_index = 0);
        ~QubitCircuit(){
//            delete []S;
 //           delete []X;
   //         delete []op_nCDecomp;
        }

        void ApplyMeasurement(std::size_t qubit, bool normalize=true);
        void ApplyNCPauliX(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target);             // Won't work ing general since measurement will destroy states in a superposition
        void ApplyNCUnitary(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, openqu::TinyMatrix<Type, 2, 2, 32> U);      // Won't work ing general since measurement will destroy states in a superposition
        void ApplyNCPauliX(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, int n);             
        void ApplyNCUnitary(vector<std::size_t> input, vector<std::size_t> ancilla, vector<std::size_t> target, openqu::TinyMatrix<Type, 2, 2, 32> U, int n);      

        void EncodeBinInToSuperposition(vector<unsigned>& reg_memory, vector<unsigned>& reg_ancilla, vector<unsigned>& bin_patterns, unsigned len_bin_pattern);
};

}
