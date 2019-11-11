# Performance Analysis

The Intel Parallel Studio Tool suite can be used to conduct performance analysis at different levels of granularity;

- Intel Trace Analyzer & Collector (ITAC): MPI tuning and analysis
- Intel VTune Amplifier: Performance profiler on a single node
- Intel Advisor: Vectorization optimization, thread prototyping and flow graph analysis

The scripts `run_MPI_itac.sh`, `run_MPI_amplxe.sh` and `run_MPI_adapt.sh` each launch an Slurm job for the respective profilers on the given executable (default is `inte-qnlp/demos/hamming_RotY/exe_demo_hamming_RotY`) and configuration.

## Running the scripts
In order to run the profiling scripts, ensure that the Intel Quantum Simulator used for building the ewxecutable is built with the following flags set; `-trace -fno-omit-frame-pointer -fPIC -g -O3`. Note: that some of these flags will inhibit performance and should be removed for production runs and proper benchmarking statistics.

After setting up the usual environment variables, execute
```
sbatch <PERFORMANCE-ANALYSIS-SCRIPT>
```

The results of the analysis will be outputted into the directory `PROFILING_RESULTS` relative to the directory `intel-qnlp/demos/perf`. The results can be viewd with the profiling type's specific GUI.