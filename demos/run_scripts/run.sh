#!/bin/bash
#SBATCH -J meas
#SBATCH -N 1
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_RESULTS_FILE=time.csv
touch ${TIMING_RESULTS_FILE}

################################################
### NUMA Bindings
################################################

NUMA_CPU_BIND="0"
NUMA_MEM_BIND="0"

NUMA_CTL_CMD_ARGS="numactl --cpubind=${NUMA_CPU_BIND} --membind=${NUMA_MEM_BIND}"

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=1
NTASKSPERNODE=16
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

#PATH_TO_EXECUTABLE=${QNLP_ROOT}/build/demos/encoding
#EXECUTABLE=exe_demo_encoding


EXE_VERBOSE=0
EXE_TEST_PATTERN=0
EXE_NUM_EXP=10000
EXE_LEN_PATTERNS=6
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Load relevant module files.
#################################################
module load  gcc/8.2.0 intel/2019u5

#################################################
### Set-up MPI environment variables.
#################################################
#export I_MPI_SHM=skx_avx512

# Increases performance
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

#################################################
### Run application using ITAC 
### (also collects timing metrics).
#################################################

MIN_EXE_NUM_EXP=10
MAX_EXE_NUM_EXP=10000

for (( EXE_NUM_EXP=${MIN_EXE_NUM_EXP}; EXE_NUM_EXP<=${MAX_EXE_NUM_EXP}; EXE_NUM_EXP=10*${EXE_NUM_EXP} )); do

    EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"
    echo -e "num iters ${EXE_NUM_EXP}"
    echo -e "mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}"
    start_time=`date +%s`

    #srun -N ${NNODES} -n ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

    mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
    end_time=`date +%s`
    runtime=$((end_time-start_time))

    echo "Execution time: ${runtime}"

    echo "${EXE_LEN_PATTERNS},${I_MPI_SHM},${EXE_NUM_EXP},${runtime}" >> ${TIMING_RESULTS_FILE}
    
done
