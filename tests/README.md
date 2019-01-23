# Tests for Intel QNLP project

## Swap test
Following the work of Zeng and Coecke (2016), the authors suggest candidate algorithms for the closest vector problem using quantum states.
The reference the work of Wiebe at al (2014), wherein the authors develop methods based upon extending the controlled swap (CSWAP) test, as well as
a modified Grover search.

As a first investigation into the use of the Intel qHiPSTER simulator, we implement a simple controlled swap gate, and use this for running
the CSWAP test, running with 6 qubits, and comparing the same, and orthogonal states.

The test is initially built as follows:
```bash
mpiicpc ./test_control_swap.cpp -L/ichec/work/ichec001/Intel-QS/build/lib/intel64 \
-I/ichec/work/ichec001/Intel-QS/build/include -std=c++11 -g -O3 -DUSE_MM_MALLOC \ 
-L$PWD -lqhipster -L${MKLROOT}/lib/intel64 -lmkl_rt -lpthread -lm -ldl \
-DSTANDALONE -DOPENQU_HAVE_MPI -DMKL_ILP64 -Wall -std=c++11 -qopenmp -o cswap
```
 and run as 
 ```bash
 mpirun -n X ./cswap
 ```
 with `n`$\lt 6$.
