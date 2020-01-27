#!/bin/bash
#SBATCH -J test
#SBATCH -N 8
#SBATCH -p ProdQ
#SBATCH -t 08:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`
TIMING_RESULTS_FILE=time_cpu_binding_comparison.csv
touch ${TIMING_RESULTS_FILE}

if [[ $# -gt 1 ]]; then
    echo "Illegal number of parameters"
    exit 2
fi

if [[ $# -eq 1 ]]; then
    LAUNCH_COMMAND=$1
else
    LAUNCH_COMMAND=srun
fi
LOOP_ITER=1

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=8
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

#PATH_TO_EXECUTABLE=${QNLP_ROOT}/build/demos/encoding
#EXECUTABLE=exe_demo_encoding

EXE_VERBOSE=0
EXE_TEST_PATTERN=0
EXE_NUM_EXP=1000
EXE_LEN_PATTERNS=10
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Load relevant module files.
#################################################
module purge
module load  gcc/8.2.0 intel/2019u5

#################################################
### Set-up MPI environment variables.
#################################################
export I_MPI_SHM=skx_avx512

# Increases performance
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

#################################################
### Run application using ITAC 
### (also collects timing metrics).
#################################################

EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#export I_MPI_DEBUG=5

if [ "${LAUNCH_COMMAND}" = "srun" ]; then
    export I_MPI_HYDRA_TOPOLIB=hwloc

    SOCKETOCCUPANCY_EVEN_NOTEVEN=EVEN_SOCKET16
    for (( i=1; i<=${LOOP_ITER}; i++ )); do

        start_time=`date +%s`

        srun --ntasks ${NPROCS} -c 1 --ntasks-per-socket=16 --cpu-bind=cores -m plane=16  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo "${LAUNCH_COMMAND},${LOOP_ITER},${EXE_NUM_EXP},${SOCKETOCCUPANCY_EVEN_NOTEVEN},${runtime}" >> ${TIMING_RESULTS_FILE}

    done

    SOCKETOCCUPANCY_EVEN_NOTEVEN=UNEVEN_SOCKET_FILL20
    for (( i=1; i<=${LOOP_ITER}; i++ )); do

        start_time=`date +%s`

        srun --ntasks ${NPROCS} -c 1 --cpu-bind=cores -m plane=40  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo "${LAUNCH_COMMAND},${LOOP_ITER},${EXE_NUM_EXP},${SOCKETOCCUPANCY_EVEN_NOTEVEN},${runtime}" >> ${TIMING_RESULTS_FILE}

    done

    SOCKETOCCUPANCY_EVEN_NOTEVEN=UNEVEN_SOCKET_32_ONNODE
    for (( i=1; i<=${LOOP_ITER}; i++ )); do

        start_time=`date +%s`

        srun --ntasks ${NPROCS} -c 1 --cpu-bind=cores -m plane=32  ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo "${LAUNCH_COMMAND},${LOOP_ITER},${EXE_NUM_EXP},${SOCKETOCCUPANCY_EVEN_NOTEVEN},${runtime}" >> ${TIMING_RESULTS_FILE}

    done


else
    for (( i=1; i<=${LOOP_ITER}; i++ )); do
        start_time=`date +%s`

        mpirun -n ${NPROCS} -ppn ${NTASKSPERNODE} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

        end_time=`date +%s`
        runtime=$((end_time-start_time))

        echo "Execution time: ${runtime}"
        echo "${LAUNCH_COMMAND},${EXE_NUM_EXP},${runtime}" >> ${TIMING_RESULTS_FILE}

    done
fi


