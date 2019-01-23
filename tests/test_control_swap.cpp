#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"

/*
 * Controlled swap decomposition taken from arXiV:1301.3727
 */
template <class Type>
void applyControlledSwap(QbitRegister<ComplexDP>& qreg, 
                            unsigned int q0, 
                            unsigned int q1, 
                            unsigned int q2){

    //Need at least 3 qubits for this operation
    std::cout << "NUMQBITS = " << qreg.nqbits << std::endl;
    std::cout << "q0 = " << q0 << "    q1 = "  << q1 << "   q2 = " << q2 << std::endl; 

    assert( qreg.nqbits > 2 );
    //The requested qubit indices must be available to use within the register range
    assert( (q0 < qreg.nqbits ) && (q1 < qreg.nqbits) && (q2 < qreg.nqbits) );
    //The qubits must be different from one another
    assert( q0 != q1 && q0 != q2 && q1 != q2);

    //V = sqrt(X)
    openqu::TinyMatrix<Type, 2, 2, 32> V;
    V(0,0) = {0.5,  0.5};
    V(0,1) = {0.5, -0.5};
    V(1,0) = {0.5, -0.5};
    V(1,1) = {0.5,  0.5};
    
    openqu::TinyMatrix<Type, 2, 2, 32> V_dag;
    V_dag(0,0) = {0.5, -0.5};
    V_dag(0,1) = {0.5,  0.5};
    V_dag(1,0) = {0.5,  0.5};
    V_dag(1,1) = {0.5, -0.5};
    
    qreg.applyCPauliX(q2, q1);
    qreg.applyControlled1QubitGate(q1, q2, V);
    qreg.applyControlled1QubitGate(q0, q2, V);
    
    qreg.applyCPauliX(q0, q1);
    qreg.applyControlled1QubitGate(q1, q2, V_dag);
    qreg.applyCPauliX(q2, q1);
    qreg.applyCPauliX(q0, q1);
}

/* The Fredkin gate (controlled swap, CSWAP) performs a measuremen-based test on two gates for equality. 
 * Three qubits are initialised in the given states, $\vert \psi \rangle$, $\vert \phi \rangle$, and $\vert 0 \rangle$,
 * where $\vert \psi \rangle$ and $\vert \phi \rangle$ are two arbitrary states. If these two arbitrary states are identical
 * then the outcome is is the classical 0. If they are not identical, then the resulting state is 1, with probability
 * $\frac{1}{2}\left( abs( \langle \phi \vert \psi \rangle )^{2} \right)$.
 *
 * The 3-qubit matrix form of this gate is given by:
 * $$
 * \textrm{CSWAP} 
 * = 
 * \begin{pmatrix} 
 * 1 & 0 & 0 & 0 & 0 & 0 & 0 & 0 \\ 
 * 0 & 1 & 0 & 0 & 0 & 0 & 0 & 0 \\ 
 * 0 & 0 & 1 & 0 & 0 & 0 & 0 & 0 \\ 
 * 0 & 0 & 0 & 1 & 0 & 0 & 0 & 0 \\ 
 * 0 & 0 & 0 & 0 & 1 & 0 & 0 & 0 \\ 
 * 0 & 0 & 0 & 0 & 0 & 0 & 1 & 0 \\ 
 * 0 & 0 & 0 & 0 & 0 & 1 & 0 & 0 \\ 
 * 0 & 0 & 0 & 0 & 0 & 0 & 0 & 1 
 * \end{pmatrix}
 * $$
 */
int main(int argc, char **argv){

  openqu::mpi::Environment env(argc, argv);
  if (env.is_usefull_rank() == false) return 0;
  // qHiPSTER has functions that simplify some MPI instructions. However, it is important
  // to keep trace of the current rank.
  int myid = env.rank();

  //As qHipster requires an even number of qubits, we double the CSWAP gate requirements and can reduntantly test
  //the calculation twice.
  int N = 6;
  std::size_t tmpSize = 0;

  {
      if (myid == 0) {
          printf("\n --- Swap Test --- \n");
          std::cout << "The 6-qubit register (gates 0 to 5) is initialized in state |000000>. \n"
                    << "We can perform two swap tests on the register using qubits {0,1,2} and {3,4,5}. \n"
                    << "Qubits 1 and 2 receive an X gate each to convert the register to |011000>. \n"
                    << "Qubits 5 receives an X gate to convert the register to |011010>. \n"
                    << "Qubits 0 and 3 have H gates applied, the swap gate, and another H. \n"
                    << "Qubits 0 and 3 are the measured qubits for the resulting outputs. \n"
                    << "Qubit 0 will map to state 0 and qubit 3 to the above overlap. \n";
      }

      // Create the state of a quantum register, having N qubits.
      // The state is initialized as a computational basis state (using the keyword "base")
      // corresponding to the index 0. The index corresponds to a N-bit integer in decimal
      // representation. With N qubits there are 2^N indices, from 0 to 2^{N-1}.
      QbitRegister<ComplexDP> psi1(N, "base", 0);

      psi1.applyHadamard(0);
      applyControlledSwap<ComplexDP>(psi1, 0, 1, 2);
      psi1.applyHadamard(0);

      psi1.applyHadamard(3);
      psi1.applyPauliX(5);
      applyControlledSwap<ComplexDP>(psi1, 3, 4, 5);
      psi1.applyHadamard(3);
      
      //Measured qubits
      unsigned int mq0=0, mq1 = 3;
      //Probabilities after measurement in the respective basis
      double p0=0., p1 = 0.;

      p0 = psi1.getProbability( mq0 );
      p1 = psi1.getProbability( mq1 );

      // Print such probability to screen, only if MPI rank=0.
      // This is done to avoid each rank to write the same information to screen.
      if (myid == 0){
          printf("The probability that qubit %d is in state |1> is %g\n", mq0, p0);
          printf("The probability that qubit %d is in state |1> is %g\n", mq1, p1);
      }
  }

}
