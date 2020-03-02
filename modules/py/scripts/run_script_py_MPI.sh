#!/bin/bash
#SBATCH -J MPI_Py
#SBATCH -N 4
#SBATCH -p ProdQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=time_run_EndToEndApp.csv
touch ${TIMING_FILE}

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=4
NTASKSPERNODE=32
NTHREADS=1
NPROCS=$(( NTASKSPERNODE*NNODES ))

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

NPROCS_PER_SOCKET=16
PLANE_FILL_WIDTH=16

#################################################
### Application configuration.
###
### Note: User may need to modify.
#################################################

PATH_TO_EXECUTABLE=.
EXECUTABLE=simple_MPI.py

EXECUTABLE_ARGS=""

#################################################
### Load relevant module files (gcc 8.2.0 and
### Intel 2019 update 5).
#################################################
module purge
module load gcc/8.2.0 intel/2019u5

#################################################
### Set-up MPI environment variables for SHM.
#################################################
export I_MPI_SHM=skx_avx512

#################################################
### Specify MPI parameter tuning model [optional]
#################################################
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

#################################################
# Run experiment
#################################################
start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=${NPROCS_PER_SOCKET} --cpu-bind=cores -m plane=${PLANE_FILL_WIDTH} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
