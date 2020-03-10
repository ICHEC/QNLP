# `run_scripts`- SLRUM batch scripts for launching application on a HPC system

The `run_scripts` directory contains the SLURM job scripts for a general C++ executable or Python script, and also for the end-to-end QNLP application implemented in a Python script.

The compute resources can be adjusted by changing the following variables;

- Number of nodes:

```{bash}
#SBATCH -N <NUM-NODES>
```

and

```{bash}
NNODES=<NUM-NODES>
```

- Number of processes per node:

```{bash}
NTASKSPERNODE=<NUM-PROCS-PER-NODE>
```

Note that the total number of processes must be a power of 2. The NUMA node configuration specified on the `srun` command may also need to be adjusted if changes are made to the abobe variables.

## General SLURM batch script job submission

The script `run_script_exe.sh` can be used to launch an application on a HPC system. The script takes no arguments and can be run on a system using SLURM by running the command

```
sbatch run_script_exe.sh
```

By defualt, the script executes the executable of the C++ application `intel-qnlp/demos/hamming_RotY/demo_hamming_RotY.cpp`. However, the script can easily be adjusted to run any built application (assuming correct environment and dependencies are also visable). To do this, adjust the variables in `run_script_exe.sh` as follows;

```{bash}
PATH_TO_EXECUTABLE=<PATH-TO-EXECUTABLE>
EXECUTABLE=<EXECUTABLE>
EXECUTABLE_ARGS="<EXECUTABLE-ARGUMENTS>"
```

Note that the variables `EXE_VERBOSE`, `EXE_TEST_PATTERN`, `$EXE_NUM_EXP` and `EXE_LEN_PATTERNS` can all be removed. 

## End-toend application SLURM batch script job submission

The run script `run_script_py_EndToEndQNLP.sh` launches the end-to-end application `intel-qnlp/modules/py/scripts/QNLP_EndToEnd_MPI.py`. It takes a mandatory argument in the form of a target corpus to be analysed. An optional argument of the number of experiments (shots) can also be supplied as the second argument (optional [default 10000]). To run the application, execute

'''{bash}
sbatch run_script_py_EndToEndQNLP.py <PATH-TO-CORPUS-FILE>/<CORPUS-FILE.TXT> <NUM-SHOTS>
```
