#!/bin/bash
#SBATCH -J runexe
#SBATCH -N 32
#SBATCH -p ProdQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=time_run_exe.csv
touch ${TIMING_FILE}

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=32
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

PATH_TO_EXECUTABLE=${QNLP_ROOT}/build/demos/hamming_RotY
EXECUTABLE=exe_demo_hamming_RotY

EXE_VERBOSE=0
EXE_TEST_PATTERN=0
EXE_NUM_EXP=1000
EXE_LEN_PATTERNS=7
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

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
### Run experiment
#################################################
start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=${NPROCS_PER_SOCKET} --cpu-bind=cores -m plane=${PLANE_FILL_WIDTH}  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN},${NNODES},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}
