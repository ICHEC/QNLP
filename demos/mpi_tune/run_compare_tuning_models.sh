#!/bin/bash
#SBATCH -J tune
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=timing_tuning_model_comparison.csv
touch ${TIMING_FILE}

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

###################################
# Unspecified tuning model
###################################

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN} (default unspecified),${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

###################################
# Best standard tuning model
###################################
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}


###################################
# ARG tuning models
###################################
for TUNING_MODEL_BIN in "@"; do
    echo -e "Tuning model ${TUNING_MODEL_BIN}"
    if test -f "${TUNING_MODEL_BIN}"; then
        export I_MPI_TUNING_BIN=${TUNING_MODEL_BIN}

        start_time=`date +%s`

        srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo -e "${I_MPI_TUNING_BIN},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

    else
        echo -e "Warning: Tuning file binary ${TUNING_MODEL_BIN} does not exist. File skipped."
    fi
done
