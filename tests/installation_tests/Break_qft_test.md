As discussed in `tests/installation_tests/qHiPSTER_installation_testing.md` and `tests/installation_tests/qHiPSTER_Installation_and_Usage_Issues.md`, qHiPSTER does not work for problem sizes that exceed $`2^{27}`$ states per process. MPI communication errors were reported. An example where this can be seen is for the `qft_test.cpp` program supplied with the Intel-QS library. To observe the error, compile the program in the standard way (using Intel 2018-u4), BigMPI and qHiPSTER. Then run the application using 36 qubits. The job output aborts, reporting MPI communication errors.

How to replicate this runtime error is detailed below. 

The `qft_test.cpp` program along with the `CMakeLists.txt` file located at `intel-qnlp/tests/installation_tests/` were used to build the application. The `run_exp.sh` script in the same directory launched a job across multiple processes. Note that one might need to specify an appropriate number of qubits (greater than 35) to observe the aforementioned error.

A sample of the above is supplied in the project's shared directory on Kay at `/ichec/work/ichec001/samples/qft_test`. This directory also contains a file (`slurm_Failure.out`) containing the expected output reporting the error observed, and another output file (`slurm_23_4_8_20.txt`) that is an example of a successful execution.

# Build the Application
The below commands build the application from a directory containing the aforementioned `qft_test.cpp` and `CMakeLists.txt` files.

```
mkdir build && cd build
module use /ichec/work/ichec001/modules
module load intel cmake3 qhipster bigmpi
CC=mpiicc CXX=mpiicpc cmake ..
make
cp exe_qft_test ..
cd ..
```
# Run the Application
To run the application, use the `run_exp.sh` job script ensuring the variables `NQUBITS`, `NNODES` and `NPROCS` are set appropriately. Also ensure the `NNODES` variable corresponds to the scripts requested node configuration. An example of a configuration which causes the run to fail is `NQUBITS=36`, `NNODES=32` and `NPROCS=64`. To run the job, simply execute

```
sbatch run_exp.sh
```

Experiments were also conducted using 64 nodes and 126 processes for 36 qubits as well as various other configurations and problem sizes, and these resulted in the same error.

An example run script that launches a smaller size resulting in success has also been supplied (`run_exp_small.sh`).