#!/bin/bash
#SBATCH -J adapt
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

PROFILING_RESULTS_PATH=PROFILING_RESULTS
ADVISOR_RESULTS_PATH=${PROFILING_RESULTS_PATH}/ADVISOR_RESULTS
EXPERIMENT_RESULTS_DIR=ADVISOR_RESULTS_${START_TIME}_job${SLURM_JOBID}

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
EXE_NUM_EXP=100
EXE_LEN_PATTERNS=4

EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Set-up Command line variables and Environment
### for Advisor.
###
### Note: User may need to modify.
#################################################

source ${ADVISOR_XE_2019_DIR}/advixe-vars.sh

#################################################
### Load relevant module files.
#################################################
module load  gcc/8.2.0 intel/2019u5

#################################################
### Set-up Command line variables and Environment
### for Advisor.
###
### Note: User may need to modify.
#################################################

ADVISOR_ARGS_SURVEY=""
ADVISOR_ARGS_TRIPCOUNTS="-flop -stacks"
ADVISOR_ARGS_MAP="-loops=total-time>0.1,loop-height=0"
ADVISOR_ARGS_DEPENDENCIES="-loops=scalar,loops-height=0,total-time>0.1"
ADVISOR_ARGS_SUITABILITY=""

#################################################
### Set-up directory for ADVISOR results.
#################################################

[ ! -d "${PROFILING_RESULTS_PATH}" ] && mkdir -p "${PROFILING_RESULTS_PATH}"
[ ! -d "${ADVISOR_RESULTS_PATH}" ] && mkdir -p "${ADVISOR_RESULTS_PATH}"
[ ! -d "${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"

#################################################
### Run application using ADVISOR 
### (also collects timing metrics).
#################################################

start_time=`date +%s`

### Intel MPI Syntax
# Survey Target
srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} -gtool "advixe-cl -collect survey ${ADVISOR_ARGS_SURVEY} -project-dir ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}:0" ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Survey Target run: Complete"
# Roofline Target
srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} -gtool "advixe-cl -collect tripcounts ${ADVISOR_ARGS_TRIPCOUNTS} -project-dir ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}:0" ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Roofline Target run: Complete"
# Maps Target
srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} -gtool "advixe-cl -collect map ${ADVISOR_ARGS_MAP} -project-dir ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}:0" ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Maps Target run: Complete"
# Dependencies Target
srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} -gtool "advixe-cl -collect dependencies ${ADVISOR_ARGS_DEPENDENCIES} -project-dir ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}:0" ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Dependencies Target run: Complete"
# Suitability Target (threading)
srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} -gtool "advixe-cl -collect suitability ${ADVISOR_ARGS_SUITABILITY} -project-dir ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}:0" ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
echo "Suitability Target run: Complete"

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"

#################################################
### Copy slurm output to log file location
#################################################

cp slurm-${SLURM_JOBID}.out ${ADVISOR_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}/slurm-${SLURM_JOBID}.out
