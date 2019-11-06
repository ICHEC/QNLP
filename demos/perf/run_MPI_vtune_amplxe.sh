#!/bin/bash
#SBATCH -J profile
#SBATCH -N 2
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

#START_TIME=`date + "%Y-%b-%d"`
START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

VTUNE_RESULTS_PATH=VTUNE_RESULTS
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
### Set-up Environment for application and VTUNE.
#################################################

source ${INTEL_PARALLELSTUDIO_PATH}/psxevars.sh

export VT_ACTIVITY=SYSTEM ON
export VT_PCTRACE=1

export OMP_NUM_THREADS=${NTHREADS}
export AFF_THREAD=${NTHREADS}
export KMP_AFFINITY=compact

#################################################
### Set-up directory for VTUNE results.
#################################################

[ ! -d "${VTUNE_RESULTS_PATH}" ] && mkdir -p "${VTUNE_RESULTS_PATH}"
[ ! -d "${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}" ] && mkdir -p "${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}"

export VT_LOGFILE_PREFIX=${VTUNE_RESULTS_PATH}/${EXPERIMENT_RESULTS_DIR}
export VT_LOGFILE_FORMAT=STF #[ASCII|STF|STFSINGLE|SINGLESTF]

#################################################
### Run application using VTUNE 
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
#!/bin/bash
#SBATCH -J qubits_full
#SBATCH -N 1

#SBATCH -p ProdQ
#SBATCH -t 1:00:00
#SBATCH -A "ichec001"
#no extra settings

NQUBITS="NA"

NNODES=1
NP=1
NTHREADS=40

APP_EXE=./exe_superposition_encoding
APP_ARGS=

module load intel

export MKL_ENABLE_INSTRUCTIONS=AVX512

source /ichec/packages/intel/2018u4/bin/compilervars.sh intel64
source /ichec/packages/intel/2018u4/vtune_amplifier_2018.4.0.573462/amplxe-vars.sh
source /ichec/packages/intel/2018u4/vtune_amplifier_2018.4.0.573462/apsvars.sh

export LD_LIBRARY_PATH=/ichec/packages/intel/2018u4/compilers_and_libraries_2018.5.274/linux/mkl/lib/intel64/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/ichec/packages/intel/2018u4/vtune_amplifier_2018.4.0.573462/lib64/:$LD_LIBRARY_PATH

export OMP_NUM_THREADS=${NTHREADS}
export MKL_NUM_THREADS=${NTHREADS}

echo "About to run"

mpirun -n ${NP} amplxe-cl --collect hotspots --result-dir vtune_survey_${NQUBITS}_${NNODES}_${NP}_${NTHREADS} --search-dir src:r=. -- ${APP_EXE} ${APP_ARGS} : -np ${NP} ${APP_EXE} ${APP_ARGS} > OUTPUT_ADVISOR_SURVEY_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}


echo "Finished"

mv slurm-${SLURM_JOBID}.out slurm_amplxe_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}.out
