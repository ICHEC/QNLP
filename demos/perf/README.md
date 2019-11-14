# Performance Analysis

The Intel Parallel Studio Tool suite can be used to conduct performance analysis at different levels of granularity;

- Intel Trace Analyzer & Collector (ITAC): MPI tuning and analysis
- Intel VTune Amplifier: Performance profiler on a single node
- Intel Advisor: Vectorization optimization, thread prototyping and flow graph analysis

The scripts `run_MPI_itac.sh`, `run_MPI_amplxe.sh` and `run_MPI_adapt.sh` each launch an Slurm job for the respective profilers on the given executable (default is `inte-qnlp/demos/hamming_RotY/exe_demo_hamming_RotY`) and configuration.

## Building the simulator
In order to run the profiling scripts, ensure that the Intel Quantum Simulator used for building the executable is built with the following flags set; `-trace -fno-omit-frame-pointer -fPIC -g -O3`. Note: that some of these flags will inhibit performance and should be removed for production runs and proper benchmarking statistics.

The local static simulator must now be built with the same flags enabled. This can be done by running the cmake with the `-DENABLE_PRFILING=ON` flag set.

```
cmake CC=mpiicc CXX=mpiicpc <PATH-TO-CMAKELISTS_FILE>/CMakeLists.txt -DENABLE_PROFILING=ON
```

Environment variables for the profiler suite need to be set by running the `psxevars.sh` script which is contained in the `parallel_studio_xe_*` directory relative to the install location of the  Intel library being used.

```
source <PATH-TO-SCRIPT>/psxevars.sh
```

Now the simmulator library is ready to be build by running

```
make
```

## Running the scripts
After setting up the usual environment variables, execute
```
sbatch <PERFORMANCE-ANALYSIS-SCRIPT>
```

The results of the analysis will be outputted into the directory `PROFILING_RESULTS` relative to the directory `intel-qnlp/demos/perf`. To view the results the appropriate GUI application for that profiling type must be used.

Note: the provided scripts use some features which were only made available in Intel 2019 releases. These scripts were only tested using Intel 2019 update 5.

## Viewing the results with appropriate GUI

When launching the appropriate GUI to view the profiled results, ensure the appropriate variable set-up script for that profiler was executed to update the environment variables after the appropriate module for Intel has been loaded (assuming some version of Intel 2019 was used), as shown below;

##vIntel Application Performance Snapshot:

Simply open the resulting `.html` files to show results.

### Intel Tace Analyser and Collector:
```
source ${VTROOT}/bin/itacvars.sh
traceanalyzer &
```
Open the corresponding results file in the GUI.

### Intel VTUNE Amplifier:
```
source ${VTUNE_AMPLIFIER_2019_DIR}/amplxe-vars.sh
amplxe-gui 
```
Open the corresponding results file in the GUI.

### Intel Advisor:
```
source ${ADVISOR_XE_2019_DIR}/advixe-vars.sh
advixe-gui
```
Open the corresponding results file in the GUI.
