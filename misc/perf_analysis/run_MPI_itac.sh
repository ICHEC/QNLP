#!/bin/bash
#SBATCH -J itac
#SBATCH -N 1
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

PROFILING_RESULTS_PATH=PROFILING_RESULTS
ITAC_RESULTS_PATH=${PROFILING_RESULTS_PATH}/ITAC_RESULTS
EXPERIMENT_RESULTS_DIR=ITAC_RESULTS_${START_TIME}_job${SLURM_JOBID}

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

EXE_VERBOSE=0
EXE_TEST_PATTERN=0
EXE_NUM_EXP=2
EXE_LEN_PATTERNS=12
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Load relevant module files.
#################################################
module load  gcc/8.2.0 intel/2019u5

# Increases performance
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

#################################################
### Set path to appropriate version of Intel 
### Parallel Studios on machine (directory which
### contains psxevars.sh).
###
### Note: User may need to modify.
#################################################

INTEL_PARALLELSTUDIO_PATH=/ichec/packages/intel/2019u5/parallel_studio_xe_2019.5.075/bin

#################################################
### Set-up Command line variables and Environment
### for ITAC.
###
### Note: User may need to modify.
#################################################

source ${INTEL_PARALLELSTUDIO_PATH}/psxevars.sh

export VT_ACTIVITY=SYSTEM ON
export VT_PCTRACE=1

#################################################
### Set-up directory for ITAC results.
#################################################

[ ! -d "${PROFILING_RESULTS_PATH}" ] && mkdir -p "${PROFILING_RESULTS_PATH}"
[ ! -d "${ITAC_RESULTS_PATH}" ] && mkdir -p "${ITAC_RESULTS_PATH}"
[ ! -d "${ITAC_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${ITAC_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"

export VT_LOGFILE_PREFIX=${ITAC_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}
export VT_LOGFILE_FORMAT=STF #[ASCII|STF|STFSINGLE|SINGLESTF]

#################################################
### Run application using ITAC 
### (also collects timing metrics).
#################################################

start_time=`date +%s`

# Standard MPI with C/C++
mpirun -trace -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

# MPI Applications in Python (require .so files to be specified (full paths might be required))
#mpiexec.hydra -trace "libVT.so libmpi.so" -n ${NPROCS} -ppn ${NTASKSPERNODE} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${VT_LOGFILE_PREFIX}/slurm-${SLURM_JOBID}.out
