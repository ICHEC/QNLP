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
NTASKSPERNODE=32
NPROCS=$(( NTASKSPERNODE*NNODES ))

module load qhipster

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

start_time=`date +%s`
srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16 python ${PATH_TO_EXECUTABLE}/${EXECUTABLE}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
