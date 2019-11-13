#!/bin/bash
#SBATCH -J profile
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

PROFILING_RESULTS_PATH=PROFILING_RESULTS
VTUNE_RESULTS_PATH=${PROFILING_RESULTS_PATH}/VTUNE_RESULTS
EXPERIMENT_RESULTS_DIR=VTUNE_RESULTS_${START_TIME}_job${SLURM_JOBID}

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=2
NTASKSPERNODE=2
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
EXE_NUM_EXP=500
EXE_LEN_PATTERNS=5

EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Modify to load appropriate intel, gcc and 
### qhipster libraries.
###
### Note: User may need to modify.
#################################################

module load qhipster

#################################################
### Set-up Command line variables and Environment
### for VTUNE.
###
### Note: User may need to modify.
#################################################

source ${VTUNE_AMPLIFIER_2019_DIR}/amplxe-vars.sh

VTUNE_ANALYSIS_TYPE=hpc-performance

VTUNE_ARGS="-knob sampling-interval=5 -knob stack-size=0 -knob collect-memory-bandwidth=true"
            #-knob  sampling-interval=5                 \
            #-knob enable-stack-collection=-true        \
            #-knob stack-size=0                         \ 
            #-knob collect-memory-bandwidth=true        \
            #-knob collect-affinity=true                \

SYMBOL_FILES_SEARCH_COMMAND="-search-dir ${QHIPSTER_DIR_INC}/../../qureg -search-dir ${QHIPSTER_DIR_INC}/../../util"

SOURCE_FILES_SEARCH_COMMAND="-search-dir ${QHIPSTER_DIR_INC}/../../qureg -search-dir ${QHIPSTER_DIR_INC}/../../util"

#################################################
### Set-up directory for VTUNE results.
#################################################

[ ! -d "${PROFILING_RESULTS_PATH}" ] && mkdir -p "${PROFILING_RESULTS_PATH}"
[ ! -d "${VTUNE_RESULTS_PATH}" ] && mkdir -p "${VTUNE_RESULTS_PATH}"
[ ! -d "${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"

#################################################
### Run application using VTUNE 
### (also collects timing metrics).
#################################################

start_time=`date +%s`

# Collect HPC-Performance Metrics
mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} -l amplxe-cl -collect ${VTUNE_ANALYSIS_TYPE} ${VTUNE_ARGS}  ${SYMBOL_FILES_SEARCH_COMMAND} ${SOURCE_FILES_SEARCH_COMMAND} -result-dir ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/profile -- ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Completeted hpc-performance"


# Collect Threading Metrics
#mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} amplxe-cl -collect threading -r ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/profile -- ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Completed threading"

# Collect Threading with Hardware counters Metrics
#mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} amplxe-cl -collect threading -knob sampling-and-waits=hw -r ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/profile -- ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Completed threading with Hardware"

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/slurm-${SLURM_JOBID}.out
