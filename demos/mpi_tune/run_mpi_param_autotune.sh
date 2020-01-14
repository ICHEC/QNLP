#!/bin/bash
#SBATCH -J meas
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=2
NTASKSPERNODE=32
NTHREADS=1
NPROCS=$(( NTASKSPERNODE*NNODES ))

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

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
### Load relevant module files.
#################################################
module purge
module load gcc/8.2.0 intel/2019u5

#################################################
### Set-up MPI environment variables for SHM.
#################################################
export I_MPI_SHM=skx_avx512

#################################################
### Run application using Automatic MPI Tuning  
#################################################
export I_MPI_TUNING_MODE=auto:application
export I_MPI_TUNING_BIN_DUMP=tuning_nn${NNODES}_np${NPROCS}_${START_TIME}.dat
export I_MPI_TUNING_AUTO_ITER_NUM=5
export I_MPI_TUNING_AUTO_SYNC=1
#export I_MPI_TUNING_AUTO_POLICY=

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

echo -e "\
# To use the tuning configuration binary generated in a production run, \ 
# execute the following command before running the application: \
# export I_MPI_TUNING_BIN=<PATH-TO-TUNING-BINARY>/<TUNING-BINARY.dat>   \
# ie. export I_MPI_TUNING_BIN=${PWD}/${I_MPI_TUNING_BIN_DUMP}   \
"
