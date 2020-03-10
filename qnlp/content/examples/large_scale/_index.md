---
title: "Distributed QNLP example"
date: 2020-03-01T10:48:38Z
draft: false
---

## The following example is available as a runnable Python script `QNLP_EndToEnd_MPI.py` in the source repository.

In this example we provide a sample usage to analyse large text documents, extract noun-verb-noun sentences, where available, and encode into states. More details will be presented on usage in a later publication.

Assuming the installation instructions have been followed for the distributed backend (see [here](install/)), then we can proceed with running the QNLP methods at scale. For this, we can use the `QNLP_EndToEnd_MPI.py` script. This file has a workflow to load and analyse a given corpus of text, tokenise, tag and determine the optimal encoding elements to represent meaning for a given set of control parameters.

## Example usage
Assuming we have access to a HPC system with the Slurm system, we can define a submission script, `job.sh` as

```bash
#!/bin/bash
#SBATCH -J <job_name>
#SBATCH -N <number_of_node>
#SBATCH -p <submission_queue>
#SBATCH -t <run_time>
#SBATCH -A <charge_account>
#SBATCH --mail-user=<email_address>
#SBATCH --mail-type=ALL

#Change to the QNLP directory, load the Intel Compiler suite and CMake3.12+, and ensure GCC 7+ compatibility
cd qnlp/build 
module load intel/2019u5 gcc cmake3

#Source the appropriate runtime variables and paths
source ../load_env.sh

# The following env. variables control the runtime of the scripts, and subsequently define the size of the simulation.
NUM_BASIS_NOUN=8 \
NUM_BASIS_VERB=2 \
BASIS_NOUN_DIST_CUTOFF=4 \
BASIS_VERB_DIST_CUTOFF=4 \
VERB_NOUN_DIST_CUTOFF=4 \
srun -n <number_of_procs> --ntasks-per-socket=<largest_power_2_cores> -c 1 --cpu-bind=cores -m plane=<largest_power_2_cores> python ../modules/py/scripts/QNLP_EndToEnd_MPI.py <corpus> <num_experiments>
cp slurm-"$SLURM_JOB_ID".out log_"$SLURM_JOB_ID".out
tar cvzf "$SLURM_JOB_ID".tar.gz ./qnlp* log_"$SLURM_JOB_ID".out
```

Note that all variables with `<>` structure must be user-defined. The runtime control parameters can also be changed, but be aware that setting them without prior knowledge may require significant computational resources. It is often best to run a resource estimation build first (`-DENABLE_RESOURCE_EST=1`) to understand the qubit and gate-count requirements for a given set of parameters.

A sample run of the above scripts for a given set of parameters and corpus choice yielded the following encoding and results output:

![Simulation example](/img/qnlp_75_50k.png "")

It is best to examine the `QNLP_EndToEnd_MPI.py` for more details, as it is well documented to explain the workflow.