#!/bin/bash
#SBATCH -J tune
#SBATCH -N 32
#SBATCH -p ProdQ
#SBATCH -t 08:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=timing_tuning_model_comparison_EndToEndApp.csv
touch ${TIMING_FILE}

if [[ $# -eq 0 ]]; then
    echo -e "Error: No command line args supplied.\nA corpus/text file needs to be provided"
    exit 1
fi

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

#################################################
### Application configuration.
###
### Note: User may need to modify.
#################################################

PATH_TO_EXECUTABLE=${QNLP_ROOT}/modules/py/scripts
EXECUTABLE=QNLP_EndToEnd_MPI.py

EXE_TARGET_CORPUS=$1 # Corpus file is expected as first command line argument.
EXECUTABLE_ARGS="${EXE_TARGET_CORPUS}"

# Application configuration parameters are set as environment variables
export NUM_BASIS_NOUN=10 
export NUM_BASIS_VERB=4
export BASIS_NOUN_DIST_CUTOFF=2
export BASIS_VERB_DIST_CUTOFF=2
export VERB_NOUN_DIST_CUTOFF=1 

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
### Run application using different tuning models
#################################################

###################################
# Unspecified tuning model
###################################

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16 python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "(default unspecified),${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

###################################
# Best standard tuning model
###################################
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16 python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

###################################
# ARG tuning models
###################################
for TUNING_MODEL_BIN in "${@:2}"; do
    echo -e "Tuning model ${TUNING_MODEL_BIN}"
    if test -f "${TUNING_MODEL_BIN}"; then
        export I_MPI_TUNING_BIN=${TUNING_MODEL_BIN}

        start_time=`date +%s`

        srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16 python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo -e "${I_MPI_TUNING_BIN},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

    else
        echo -e "Warning: Tuning file binary ${TUNING_MODEL_BIN} does not exist. File skipped."
    fi
done
