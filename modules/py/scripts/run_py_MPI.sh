#!/bin/bash
#SBATCH -J MPI_Py
#SBATCH -N 4
#SBATCH -p ProdQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

PATH_TO_EXECUTABLE=.
EXECUTABLE=simple_MPI.py

NNODES=4
NTASKSPERNODE=2
NTHREADS=1
NPROCS=$(( NTASKSPERNODE*NNODES ))

module load qhipster

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

start_time=`date +%s`
srun -N ${NNODES} -n ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} -c ${NTHREADS} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
