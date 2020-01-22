#!/bin/bash
#SBATCH -J tune
#SBATCH -N 32
#SBATCH -p ProdQ
#SBATCH -t 10:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

if [[ $# -eq 0 ]]; then
    echo -e "Error: No command line args supplied.\nAn corpus/text file needs to be provided"
    exit 1
elif [[ $# -gt 1 ]]; then
    echo -e "Error: Too many command line arguments supplied. $# supplied, but expected 1."
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

EXE_TARGET_CORPUS=$1 # Corpus file is expected as command line argument.
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
### Run application using Automatic MPI Tuning  
#################################################
export I_MPI_TUNING_MODE=auto:application
export I_MPI_TUNING_BIN_DUMP=tuning_EndToEndApp_nn${NNODES}_np${NPROCS}_${START_TIME}.dat
export I_MPI_TUNING_AUTO_ITER_NUM=1
export I_MPI_TUNING_AUTO_SYNC=1
#export I_MPI_TUNING_AUTO_POLICY=

start_time=`date +%s`

srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16 python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

echo -e "\
# To use the tuning configuration binary generated in a production run, \n 
# execute the following command before running the application: \n
# export I_MPI_TUNING_BIN=<PATH-TO-TUNING-BINARY>/<TUNING-BINARY.dat>   \n
# ie. export I_MPI_TUNING_BIN=${PWD}/${I_MPI_TUNING_BIN_DUMP}   \n
"
