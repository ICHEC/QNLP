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

PATH_TO_EXECUTABLE=../../build/demos/hamming_RotY
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

VTUNE_ANALYSIS_TYPE=hpc-performance

VTUNE_ARGS="
            "
            #-knob enable-stack-collection=true  \
            #-knob collect-memory-bandwidth=true \
            #-knob collect-affinity=true         \

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

mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} amplxe-cl -collect ${VTUNE_ANALYSIS_TYPE} ${VTUNE_ARGS} -r ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/profile -- ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/slurm-${SLURM_JOBID}.out
