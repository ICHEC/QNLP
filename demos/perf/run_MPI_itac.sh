#!/bin/bash
#SBATCH -J profile
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

#START_TIME=`date + "%Y-%b-%d"`
START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

ITAC_RESULTS_PATH=ITAC_RESULTS
EXPERIMENT_RESULTS_DIR=ITAC_RESULTS_${START_TIME}_job${SLURM_JOBID}

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=2
NTASKSPERNODE=2
NTHREADS=1
NPROCS=$(( NTASKSPERNODE*NNODES ))

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
EXE_LEN_PATTERNS=4
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Modify to load appropriate intel, gcc and 
### qhipster libraries.
###
### Note: User may need to modify.
#################################################

module load qhipster

#################################################
### Set path to appropriate version of Intel 
### Parallel Studios on machine (directory which
### contains psxevars.sh).
###
### Note: User may need to modify.
#################################################

INTEL_PARALLELSTUDIO_PATH=/ichec/packages/intel/2019u5/parallel_studio_xe_2019.5.075/bin

#################################################
### Set-up Environment for application and ITAC.
#################################################

source ${INTEL_PARALLELSTUDIO_PATH}/psxevars.sh

export VT_ACTIVITY=SYSTEM ON
export VT_PCTRACE=1

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

#################################################
### Set-up directory for ITAC results.
#################################################

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
mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
#mpirun -trace -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

# MPI Applications in Python (require .so files to be specified (full paths might be required))
#mpiexec.hydra -trace "libVT.so libmpi.so" -n ${NPROCS} -ppn ${NTASKSPERNODE} python ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${VT_LOGFILE_PREFIX}/slurm-${SLURM_JOBID}.out
