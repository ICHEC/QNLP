# Strong Scaling Experiments

This directory contains a SLURM run script for launching strong scaling experiments. The application used for the scaling experiments is the Python script `${QNLP_ROOT}/intel-qnlp/modules/py/scripts/QNLP_EndToEnd_MPI.py`. This SLURM script takes a single command line argument; a corpus text which is to be analysed. Further application configurations can be adjusted in the SLURM job script and in the Python script.

The application is launched on a varying number of nodes (and hence processes) as the problem sized is kept fixed. The default run uses 32 processes per node. The application is run using a varying number of nodes from 4 to 32, restricting the number of nodes to be a power of 2 ({4,8,16,32}). The range of nodes the experiment is repeated for can be adjusted in the SLRUM script along with other parameters such as the NUMA node configuration, and the MPI parameter configuration.

To launch the batch script on a system using the  SLURM scheduler, execute

```{bash}
sbatch run_strong_scaling_EndToEndQNLP.sh <PATH-TO-CORPUS-FILE>/<CORPUS-FILE.TXT>
```

The generated output files are stored in the specified subdirectory `${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}` which can be defined within the SLURM script. This directory will be created if it does not already exist. Along with the generated output files will be a file named `${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/time_run_strong_scaling_EndToEndApp`. This file contains the execution times of each of the experiments along with the configuration details of that experiment. These timing results can be plotted to view the strong scaling performance. 
