# qHiPSTER Installation Process Issues

The Intel-QS ‘consistent-naming’ branch of qHiPSTER is currently installed using Intel Parallel Studio XE 2018 update 4 in the shared project directory on Kay (`/ichec/work/ichec001/<BUILD_DIR>`).

Note, BigMPI is required to allow for larger MPI message sizes which occurs when the number of states per process becomes relatively large (approximately greater than $`2^{27}`$ states per process). BigMPI had to be built seperately within the Intel-QS installation. 

For further details of the installation procedure and open issues with the procedure, see the qHiPSTER_build_process.md document (https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/qHiPSTER_build_process.md). It details the installation procedure on Kay, including workarounds so that the installation will be successful. A sumamary of the critical issues which prevent successful installation of the SDK-release of qHiPSTER are as follows:

- Missing `LICENSE.txt` file prevents build process from starting
- After build is complete, `NoisyQureg.hpp` is missing from the build. It must be manually added to `/<SDK_BUILD_DIR>/include/qureg` or one can adjust the `Makefile` to ensure the copy of the file into the SDK-release's build.

A less critical issue, but would be nice to have, would be to let the user specify whether the build process shoud use stanard MPI or BigMPI, and to build accordingly. At the moment BigMPI must be installed separately first.

To further the above, certain paths in the `make.inc`, namely `BIGMPI_LIB` and `BIGMPI_INC` file needs to be adjusted as it assumes that BigMPI has been pre-installed on the machine. An automated build process would make this a lot simpler and user friendly.

Including unit tests would be nice addition to have as well.


# Using QHiPSTER on Kay
- Load the default `intel` module (Intel Parallel Studio XE 2018 update 4)
- Add the paths to the custom `qhipster` and `bigmpi` module files to the modulepath (`module use /ichec/work/ichec001/modules`
- Load the custom modules `qhipster` and `bigmpi` which are located at `/ichec/work/ichec001/modules/<MODULE_NAME>`.
- Ensure the required Intel compiler flags are set (see https://software.intel.com/en-us/articles/intel-mkl-link-line-advisor for details)
- Set the following precompiler flags `-DOPENQU_HAVE_MPI`, `-DINTELQS_HAS_MPI` and if using BigMPI then set `-DBIGMPI`.
- Ensure the qHiPSTER and BigMPI static libraries are linked correctly and their appropriate include files are visible.

An example build of the `qft_test.cpp` program along with the associated `CMakeLists.txt` file can be seen in the project's shared directory `/ichec/work/ichec001/samples/qft_test` on Kay. Note that the `CMakeLists.txt` file used requires CMake version 3.0 or newer. The script `run_script.sh` can be used to launch a SLURM job once the executable has been made (`run_script_small.sh` can be used to launch a small job on the `DevQ`). 


# Scaling on Kay

The `qft_test.cpp` program supplied with the Intel-QS installation was used to test scaling on Kay. The program was copied to a directory that was independent of the installation. A `CMakeLists.txt` file was created to correctly make the `Makefile` and is a good reference if attempting to compile a program using BigMPI and qHiPSTER together.

The `qft_test.cpp` executable was generated successfully. The script `run_script.sh` was then used to launch jobs on Kay using SLURM for different problem sizes (numbers of qubits) and for different node configurations. Note, that for the node configurations, only two processes were allocated per node, each with 20 threads. The number of processes was restricted by the Intel-QS framework to be in powers of two. Therefore, the scaling was performed for $`2,4,8,16,32,64`$ and $`128`$ processes.

## Strong Scaling
Strong scaling was performed keeping the problem size fixed to $`34`$ qubits and varying the number of processes according to the aforementioned configurations.

| Nodes           |  p    | NumProcesses = $`2^p`$  | Local States $`= 2^{n-p} = 2^{27}`$  | Qubits = $`n`$ |
| :-------------: | :---: | :---------------------: | :----------------------------------: | :------------: |
| 64              |   7   | 128                   | $`2^{34-7}`$                        | 34           |
| 32              |   6   | 64                    | $`2^{34-6}`$                         | 34           |
| 16              |   5   | 32                    | $`2^{34-5}`$                         | 34           |
| 8               |   4   | 16                    | $`2^{34-4}`$                         | 34           |
| 4               |   3   | 8                     | $`2^{34-3}`$                         | 34           |
| 2               |   2   | 4                     | $`2^{34-2}`$                         | 34           |
| 1               |   1   | 2                     | $`2^{34-1}`$                        | 34           |



![scaling_strong_.png](tests/Installation_Tests/scaling_strong_.png)
![scaling_speedup_strong.png](tests/Installation_Tests/scaling_speedup_strong.png)

From the above plots, it is clear that there is no significant reduction in runtime when the number of ranks exceeds $`36`$ (approximately the number of qubits). It would be beneficial to conduct scaling experiments for larger problem sizes, however this is limited due to the maximum message passing size since BigMPI is not being used by qHiPSTER. The problem scales strongly quite well.

## Weak Scaling
Weak scaling was performed keeping the problem size assigned to each task fixed to $`2^{27}`$ states. The job configurations and how they were calculated can be seen in the table below.

| Nodes           |  p    | NumProcesses = $`2^p`$    | Local States $`= 2^{n-p} = 2^{27}`$    | Qubits = $`n`$   |
| :-------------: | :---: | :---------------------: | :----------------------------------: | :------------: |
| 64              |   7   | 128                     | $`2^{34-7}`$                           | 34             |
| 32              |   6   | 64                      | $`2^{33-6}`$                          | 33             |
| 16              |   5   | 32                      | $`2^{32-5}`$                           | 32             |
| 8               |   4   | 16                      | $`2^{31-4}`$                           | 31             |
| 4               |   3   | 8                       | $`2^{30-3}`$                          | 30             |
| 2               |   2   | 4                       | $`2^{29-2}`$                           | 29             |
| 1               |   1   | 2                       | $`2^{28-1}`$                           | 28             |



![scaling_weak_.png](tests/Installation_Tests/scaling/scaling_weak_.png)
![scaling_speedup_weak.png](tests/Installation_Tests/scaling/scaling_speedup_weak.png)


From the above plots for weak scaling, it can be seen that the program scales resonably well. The execution time increases as the number of processes is increased, likely due to MPI communication overhead. It would be worthwhile repeating weak scaling experiments with BiGMPI working so as to compare the two for smaller problem sizes. BiGMPI should also be used for larger numbers of processes in terms of weak scaling.


# Main Issues
- Previously mentioned installation issues
- Lack of documentation for use as an SDK
- Consider pushing `consistent-naming` branch to master since it is more up to date and names of routines are consistent


# Summary of Current Status

 - Working off of `consistent-naming` branch,
 - Built BigMPI
 - Built SDK-release of Intel-QS using BigMPI
 - Can successfully build and run programs in HPC environment
 - `qft_test.cpp` works for problem sizes with the number of states per process equal to $`2^{28}`$ (16GiB)
 - `qft_test.cpp` fails for problem sizes with the number of states per process greater than $`2^{28}`$ states
     - e.g. 36 qubits - single precision - Node Configuration: 64 nodes, 128 processes, 20 threads per process - fails due to communication error
     - This is a strong indicator that BigMPI is not being used for all of the appropriate MPI routines.

# Questions
- Are `MPIX_Sendrecv_x`and `MPIX_Allreduce_x` the only BigMPI routines required by qHiPSTER? (as they appear to be the only BigMPI routines implemented by it (by grep'ing the Intel-QS source code, these were the only `MPIX_` routines)). See `Intel-QS/build/include/util/utils.hpp` to see where the MPI routines are set to use Big MPI by specifying the `-DBIGMPI` precompiler flag.
- Do any of the branches on the Intel-QS repository fully implement BigMPI when required?
- In the `Intel-QS/tests` directory, all of the MPI routines are not implemented due to the dependency that the `DNOREPA_HAS_MPI` precompiler flag has been defined (which was not included in the build process). Is there a reason why MPI was disabled by default for the tests? (this question applies to the `consistent-naming` branch only)

# Current Roadblocks
- Major roadblock is that BigMPI does not seem to be fully implemented in the installation. This restricts the NLP algorithm to only use a small and less dissimilar corpus of words.
