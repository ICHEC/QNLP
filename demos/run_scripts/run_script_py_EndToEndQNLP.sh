#!/bin/bash
#SBATCH -J runpy
#SBATCH -N 2
#SBATCH -p ProdQ
#SBATCH -t 24:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_FILE=time_run_EndToEndApp.csv
touch ${TIMING_FILE}

if [[ $# -eq 0 ]]; then
    echo -e "Error: No command line args supplied.\nA corpus/text file needs to be provided"
    exit 1
elif [[ $# -gt 2 ]]; then
    echo -e "Error: Too many command line arguments supplied. $# supplied, but expected 1 or 2 (input_file.txt [FILE], num_shots [optional, integer] respectively)."
    exit 1
fi


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

NPROCS_PER_SOCKET=16
PLANE_FILL_WIDTH=16

#################################################
### Application configuration.
###
### Note: User may need to modify.
#################################################

PATH_TO_EXECUTABLE=${QNLP_ROOT}/modules/py/scripts
EXECUTABLE=QNLP_EndToEnd_MPI.py

EXE_TARGET_CORPUS=$1 # Corpus file is expected as command line argument.
if [ ! -f ${EXE_TARGET_CORPUS} ]; then
    echo "Error: Inputted corpus file '${EXE_TARGET_CORPUS}' not found!"
    exit 1
fi

if [[ $# -eq 2 ]]; then
    EXE_NUM_SHOTS=$2
else
    EXE_NUM_SHOTS=10000
fi

EXECUTABLE_ARGS="${EXE_TARGET_CORPUS} ${EXE_NUM_SHOTS}"

# Application configuration parameters are set as environment variables
export NUM_BASIS_NOUN=10 
export NUM_BASIS_VERB=4
export BASIS_NOUN_DIST_CUTOFF=5
export BASIS_VERB_DIST_CUTOFF=5
export VERB_NOUN_DIST_CUTOFF=4

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
start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=${NPROCS_PER_SOCKET} --cpu-bind=cores -m plane=${PLANE_FILL_WIDTH} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_BIN},${NNODES},${NPROCS},${EXECUTABLE_ARGS},${runtime}" >> ${TIMING_FILE}

