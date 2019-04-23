#!/bin/bash
#SBATCH -J qubits_full
#SBATCH -N 2

#SBATCH -p DevQ
#SBATCH -t 00:20:00
#SBATCH -A "ichec001"
#no extra settings

NQUBITS=23

NNODES=2
NPROCS=4
NTHREADS=20



#module load qhipster
module load intel
#module load bigmpi

export OMP_NUM_THREADS=${NTHREADS}
srun -N ${NNODES} -n ${NPROCS} -c ${NTHREADS} ./build/exe_qft_test ${NQUBITS}

mv slurm-${SLURM_JOBID}.out slurm_${NQUBITS}_${NNODES}_${NPROCS}_${NTHREADS}.out
