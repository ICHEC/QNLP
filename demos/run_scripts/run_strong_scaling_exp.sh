#!/bin/bash
#SBATCH -J scaling_exp
#SBATCH -N 1
#SBATCH -p ProdQ
#SBATCH -t 24:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

SCALING_TYPE=STRONG

SCALING_RESULTS_PATH=SCALING_RESULTS_original
EXPERIMENT_RESULTS_DIR=STRONG_SCALING_RESULTS_${START_TIME}_job${SLURM_JOBID}
TIMING_FILE=timing_original.csv

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

MIN_NNODES=1
MAX_NNODES=${MIN_NNODES}
NTASKSPERNODE=8

#################################################
### Threading and core affinity.
###
### Note: User may need to modify.
#################################################

NTHREADS=1

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

VECTORISATION=AVX512

#################################################
### Application configuration.
###
### Note: User may need to modify.
#################################################

PATH_TO_EXECUTABLE=${QNLP_ROOT}/build_scaling/demos/hamming_RotY
EXECUTABLE=exe_demo_hamming_RotY

EXE_VERBOSE=0
EXE_TEST_PATTERN=0
EXE_NUM_EXP=500

#################################################
### Modify to load appropriate intel, gcc and 
### qhipster libraries.
###
### Note: User may need to modify.
#################################################

module load qhipster

#################################################
### Set-up directory for results.
#################################################

[ ! -d "${SCALING_RESULTS_PATH}" ] && mkdir -p "${SCALING_RESULTS_PATH}"
[ ! -d "${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"
touch ${SCALING_RESULTS_PATH}/${TIMING_FILE}

#################################################
### Run application using ITAC 
### (also collects timing metrics).
#################################################


EXE_LEN_PATTERNS=8
MIN_EXE_LEN_PATTERNS=8
MAX_EXE_LEN_PATTERNS=8

NNODES=${MIN_NNODES}

#for (( NNODES=${MIN_NNODES}; NNODES<=${MAX_NNODES}; NNODES*=2 )); do
for (( EXE_LEN_PATTERNS=${MIN_EXE_LEN_PATTERNS}; EXE_LEN_PATTERNS<=${MAX_EXE_LEN_PATTERNS}; EXE_LEN_PATTERNS+=1 )); do

    NPROCS=$(( NTASKSPERNODE*NNODES ))


    #POSTFIX_ID=n${NNODES}_ppn${NTASKSPERNODE}_np${NPROCS}-${SLURM_JOBID}

    POSTFIX_ID=n${NNODES}_ppn${NTASKSPERNODE}_np${NPROCS}_len${EXE_LEN_PATTERNS}-${SLURM_JOBID}

    EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"


    start_time=`date +%s`

    mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS} &> ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/output_${POSTFIX_ID}.out

    end_time=`date +%s`
    runtime=$((end_time-start_time))

#     STORAGE_PER_NODE=$( grep -m 1 "storage per node" ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/output_${POSTFIX_ID}.out | grep -Eo '[0-9]+([.][0-9]+)?+' )
#     STORAGE_PER_STATE=$( grep -m 1 "storage per state" ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/output_${POSTFIX_ID}.out | grep -Eo '[0-9]+([.][0-9]+)?+' )
#     STORAGE_PER_NODE_TEMPORARY=$( grep -m 1 "temporary storage" ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/output_${POSTFIX_ID}.out | grep -Eo '[0-9]+([.][0-9]+)?+' )

    echo -e "${SCALING_TYPE},${NNODES},${NTASKSPERNODE},${NPROCS},${NTHREADS},${KMP_AFFINITY},${VECTORISATION},${EXE_LEN_PATTERNS},${EXE_NUM_EXP},${EXE_TEST_PATTERN},${STORAGE_PER_NODE},${STORAGE_PER_STATE},${STORAGE_PER_NODE_TEMPORARY},${runtime}" >> ${SCALING_RESULTS_PATH}/${TIMING_FILE}

    
    echo -e "${SCALING_TYPE},${NNODES},${NTASKSPERNODE},${NPROCS},${NTHREADS},${KMP_AFFINITY},${VECTORISATION},${EXE_LEN_PATTERNS},${EXE_NUM_EXP},${EXE_TEST_PATTERN},${STORAGE_PER_NODE},${STORAGE_PER_STATE},${STORAGE_PER_NODE_TEMPORARY},${runtime}"
done

echo "Execution time: ${runtime}"


#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/slurm_n${NNODES}_ppn${NTASKSPERNODE}_np${NPROCS}-${SLURM_JOBID}.out

