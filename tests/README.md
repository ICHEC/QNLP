# Tests for Intel QNLP project

## Swap test
Following the work of Zeng and Coecke (2016), the authors suggest candidate algorithms for the closest vector problem using quantum states.
The reference the work of Wiebe at al (2014), wherein the authors develop methods based upon extending the controlled swap (CSWAP) test, as well as
a modified Grover search.

As a first investigation into the use of the Intel qHiPSTER simulator, we implement a simple controlled swap gate, and use this for running
the CSWAP test, running with 6 qubits, and comparing the same (qubits 0,1,2), and orthogonal (qubits 3,4,5) states.

The test is initially built as follows:
```bash
mpiicpc ./test_control_swap.cpp -L/ichec/work/ichec001/Intel-QS/build/lib/intel64 \
    -I/ichec/work/ichec001/Intel-QS/build/include -std=c++11 -g -O3 -DUSE_MM_MALLOC \
    -l:qHiPSTER.a -L${MKLROOT}/lib/intel64 -lmkl_rt -lpthread -lm -ldl \
    ${MKLROOT}/lib/intel64/libmkl_scalapack_ilp64.a \
    -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_cdft_core.a \
    ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a \
    ${MKLROOT}/lib/intel64/libmkl_intel_thread.a \
    ${MKLROOT}/lib/intel64/libmkl_core.a \
    ${MKLROOT}/lib/intel64/libmkl_blacs_intelmpi_ilp64.a -Wl,--end-group -liomp5 \
    -lpthread -lm -ldl -DSTANDALONE -DOPENQU_HAVE_MPI -DMKL_ILP64 -Wall \
    -std=c++11 -qopenmp -o cswap
```
 and run as 
 ```bash
 mpirun -n X ./cswap
 ```
 with `n`$`\lt 6`$.

The test ends with a measurement of the states of qubits 0 and 3, and gives the resulting probability of being in |1>.

The register is initialised to state $`\vert 000000 \rangle`$, with the qubits 1,2 and 4 being operated on by an X gate.
As qubits 1 and 2 are the same, the result of a meaurement will be $`P_{12}(\vert 1 \rangle) = 0`$  with the state given by $`(1\vert 0 \rangle + 0\vert 1 \rangle)`$.
Qubits 4 and 5 differ, and are fully orthogonal, and will result in a value of $`P_{45}(\vert 1 \rangle) = 0.5`$ with the state $`1/\sqrt{2}(\vert 0 \rangle + \vert 1 \rangle)`$.

Running the code produces the following result:
```bash
The probability that qubit 0 is in state |1> is 0
The probability that qubit 3 is in state |1> is 0.5
```
indicating that our test works as expected.