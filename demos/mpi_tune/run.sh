#!/bin/bash
#SBATCH -J meas
#SBATCH -N 1
#SBATCH -p DevQ
#SBATCH -t 01:00:00
#SBATCH -A "ichec001"
#no extra settings

START_TIME=`date '+%Y-%b-%d_%H.%M.%S'`

################################################
### NUMA Bindings
################################################

NUMA_CPU_BIND="0"
NUMA_MEM_BIND="0"

#NUMA_CTL_CMD_ARGS="numactl --cpubind=${NUMA_CPU_BIND} --membind=${NUMA_MEM_BIND}"

#################################################
### MPI job configuration.
###
### Note: User may need to modify.
#################################################

NNODES=1
NTASKSPERNODE=16
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
EXE_NUM_EXP=10000
EXE_LEN_PATTERNS=6
EXECUTABLE_ARGS="${EXE_VERBOSE} ${EXE_TEST_PATTERN} ${EXE_NUM_EXP} ${EXE_LEN_PATTERNS}"

#################################################
### Load relevant module files.
#################################################
module load  gcc/8.2.0 intel/2019u5

#################################################
### Set-up MPI environment variables for SHM.
#################################################
#export I_MPI_SHM=skx_avx512
#export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_generic_shm-ofi.dat
#export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi.dat
#export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat

#################################################
### Generate list of comma separated hostnames
#################################################
TMP=$( srun hostname | sort -n )
HOSTNAMES=$( sed "s/ /,/g" <<< ${TMP} )

#################################################
### Run application using ITAC 
### (also collects timing_10000 metrics).
#################################################

#I_MPI_TUNING_MODE=auto:application

#------------------------
start_time=`date +%s`

#srun --ntasks ${NPROCS} --ntasks-per-node ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
#mpitune  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
#mpitune -np ${NPROCS} -ppn ${NTASKSPERNODE} -hosts ${HOSTNAMES} -m collect -c sample_legacy_format.cfg
# ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}

end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_generic_shm-ofi.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
#------------------------

I_MPI_TUNING_MODE=auto:application

#------------------------
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_generic_shm-ofi.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

#------------------------
export I_MPI_TUNING_BIN=${I_MPI_ROOT}/intel64/etc/tuning_skx_shm-ofi_efa.dat
start_time=`date +%s`
mpirun  -n ${NPROCS} -ppn ${NTASKSPERNODE} ${NUMA_CTL_CMD_ARGS} ${PATH_TO_EXECUTABLE}/${EXECUTABLE} ${EXECUTABLE_ARGS}
end_time=`date +%s`
runtime=$((end_time-start_time))

echo "Execution time: ${runtime}"
echo -e "${I_MPI_TUNING_MODE},${I_MPI_TUNING_BIN},${runtime}" >> timing_10000.txt

