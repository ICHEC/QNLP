#!/bin/bash
#SBATCH -J QNLP
#SBATCH -N <NUM_NODES>
#SBATCH -p <Queue>
#SBATCH -t 1-23:59:59
#SBATCH -A <Account name>
#SBATCH --mail-user=<email address>
#SBATCH --mail-type=ALL

cd /path/to/qnlp/build 

#Ensure correct modules are loaded
module load intel/2019u5 gcc

#Set up environment
source ../load_env.sh

#Ensure these numbers are powers of 2 for best performance
NUM_CORES_PER_SOCKET = <num_cores>
NUM_PROCS_PER_NODE = <NUM_CORES_PER_SOCKET * num_sockets_per_node>
NUM_PROCS=<NUM_NODES * NUM_CORES_PER_SOCKET>

# Program name:
# PROG=./QNLP_End2End_MPI.py
# The above program performs all preprocessing, encodes the preprocessed data, and compares a test state with the encoded meaning-space.
# The comparison adjusts the amplitudes of the encoded states, and the resulting distribution is sampled the requested number of times.

# PROG=./QNLP_Overlap.py
# The above program performs all preprocessing, encodes the preprocessed data, and compares a test state with the encoded meaning-space for two simulators.
# All potential, but not encoded patterns are then assembled as test patterns; each pair of test patterns is mapped to the state as with QNLP_End2End_MPI.py,
# but the fidelities of the two distributions are now compared and returned. This will allow us to determine similarity of separate patterns encoded in
# a meaning-space determined by the given corpus. Sampling should only be performed once for this case.

PROG=./QNLP_End2End_MPI.py
NUM_SAMPLES=1000

# The corpus to analyse
CORPUS=/path/to/qnlp/corpus/AIW_PG.txt

#Define runtime parameters for preprocessing stage and run job
NUM_BASIS_NOUN=8 \
NUM_BASIS_VERB=4 \
BASIS_NOUN_DIST_CUTOFF=5 \
BASIS_VERB_DIST_CUTOFF=5 \
VERB_NOUN_DIST_CUTOFF=4 \
srun -n $NUM_PROCS --ntasks-per-socket=$NUM_CORES_PER_SOCKET -c 1 --cpu-bind=cores -m plane=$NUM_PROCS_PER_NODE python $PROG $CORPUS $NUM_SAMPLES
cp slurm-"$SLURM_JOB_ID".out log_"$SLURM_JOB_ID".out
tar cvzf "$SLURM_JOB_ID".tar.gz ./*.pdf ./*.pkl og_"$SLURM_JOB_ID".out
