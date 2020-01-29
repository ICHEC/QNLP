#!/bin/bash
#SBATCH -J strong
#SBATCH -N 32
#SBATCH -p ProdQ
#SBATCH -t 24:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=time_run_strong_scaling_EndToEndApp.csv

SCALING_TYPE=STRONG
SCALING_RESULTS_PATH=SCALING_RESULTS
EXPERIMENT_RESULTS_DIR=STRONG_SCALING_RESULTS_${START_TIME}_job${SLURM_JOBID}

if [[ $# -eq 0 ]]; then
    echo -e "Error: No command line args supplied.\nA corpus/text file needs to be provided.\nFurther optional arguments of different MPI Autotuning configuration files can be provided."
    exit 1
fi

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################
MAX_NNODES=32
MIN_NNODES=4

NTASKSPERNODE=32
NTHREADS=1
NPROCS=$(( NTASKSPERNODE*NNODES ))

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact
VECTORISATION=AVX512

NPROCS_PER_SOCKET=16
PLANE_FILL_WIDTH=16

# Check validity of max and min number of nodes (must be power of 2).
if (( ${MAX_NNODES} % 2)) && (( ${MAX_NNODES} != 1 )): then
    echo -e "The maximum number of nodes supplied (MAX_NNODES = ${MAX_NNODES}) is not a power of 2. MAX_NNODES must be a power of 2"
    exit 1
elif (( ${MIN_NNODES} % 2)) && (( ${MIN_NNODES} != 1 )): then
    echo -e "The maximum number of nodes supplied (MAX_NNODES = ${MAX_NNODES}) is not a power of 2. MAX_NNODES must be a power of 2"
    exit 1
fi


#################################################
### Application configuration.
###
### Note: User may need to modify.
#################################################
PATH_TO_EXECUTABLE=${QNLP_ROOT}/modules/py/scripts
EXECUTABLE=QNLP_EndToEnd_MPI.py
GENERATED_RESULTS="qnlp_e2e.pdf  qnlp_e2e.pkl"

EXE_TARGET_CORPUS=$1 # Corpus file is expected as first command line argument.
EXECUTABLE_ARGS="${EXE_TARGET_CORPUS}" # List of applicatiopn arguments.

# Application configuration parameters are set as environment variables
export NUM_BASIS_NOUN=10 
export NUM_BASIS_VERB=4
export BASIS_NOUN_DIST_CUTOFF=2
export BASIS_VERB_DIST_CUTOFF=2
export VERB_NOUN_DIST_CUTOFF=1 

#################################################
### Set-up directory for results.
#################################################
[ ! -d "${SCALING_RESULTS_PATH}" ] && mkdir -p "${SCALING_RESULTS_PATH}"
[ ! -d "${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"
touch ${SCALING_RESULTS_PATH}/${TIMING_FILE}


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

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN},${NNODES},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

#################################################
### Run application scaling
#################################################
TOTAL_EXE_TIME=0.0

for (( NNODES=${MAX_NNODES}; NNODES>=${MIN_NNODES}; NNODES/=2 )); do

    NPROCS=$(( NTASKSPERNODE*NNODES ))

    PREFIX_ID=n${NNODES}_ppn${NTASKSPERNODE}_np${NPROCS}_len${EXE_LEN_PATTERNS}-${SLURM_JOBID}

    start_time=`date +%s`

    srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=${NPROCS_PER_SOCKET} --cpu-bind=cores -m plane=${PLANE_FILL_WIDTH} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

    end_time=`date +%s`
    runtime=$((end_time-start_time))

    for FILE in ${GENERATED_RESULTS}; do
        mv ${FILE} ${SCALING_RESULTS_PATH}/output_${PREFIX_ID}_${FILE}
    done

    echo -e "${SCALING_TYPE},${NNODES},${NTASKSPERNODE},${NPROCS},${NTHREADS},${KMP_AFFINITY},${VECTORISATION},${VECTORISATION},${EXE_LEN_PATTERNS},${EXE_NUM_EXP},${EXE_TEST_PATTERN},${STORAGE_PER_NODE},${EXECUTABLE_ARGS},${runtime}" >> ${SCALING_RESULTS_PATH}/${TIMING_FILE}
    echo -e "${SCALING_TYPE},${NNODES},${NTASKSPERNODE},${NPROCS},${NTHREADS},${KMP_AFFINITY},${VECTORISATION},${VECTORISATION},${EXE_LEN_PATTERNS},${EXE_NUM_EXP},${EXE_TEST_PATTERN},${STORAGE_PER_NODE},${EXECUTABLE_ARGS},${runtime}"

    TOTAL_EXE_TIME+=${runtime}
done

echo "Total Execution time: ${TOTAL_EXE_TIME}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${SCALING_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/.
