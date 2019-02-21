#!/bin/bash
#SBATCH -J qubits_full
#SBATCH -N 8

#SBATCH -p ProdQ
#SBATCH -t 1:00:00
#SBATCH -A "ichec001"
#no extra settings

NQUBITS=28

NNODES=8
NP=16
NTHREADS=20

APP_EXE=./exe_qft_test
APP_ARGS=${NQUBITS}

#module load qhipster
module load intel
#module load bigmpi

export MKL_ENABLE_INSTRUCTIONS=AVX512

source /ichec/packages/intel/2018u4/bin/compilervars.sh intel64
#source /ichec/packages/intel/2018u4/vtune_amplifier_2018.4.0.573462/amplxe-vars.sh

source /ichec/packages/intel/2018u4/parallel_studio_xe_2018.4.057/psxevars.sh

#export LD_LIBRARY_PATH=/ichec/packages/intel/2018u4/compilers_and_libraries_2018.5.274/linux/mkl/lib/intel64/:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=/ichec/packages/intel/2018u4/vtune_amplifier_2018.4.0.573462/lib64/:$LD_LIBRARY_PATH

export OMP_NUM_THREADS=${NTHREADS}
export MKL_NUM_THREADS=${NTHREADS}

echo "About to run"

mpirun -genv VT_LOGFILE_FORMAT=SINGLESTF -trace -n ${NP} ./${APP_EXE} ${APP_ARGS} > OUTPUT_ITAC_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}
#mpirun -n ${NP} amplxe-cl --collect threading --result-dir vtune_survey_${NQUBITS}_${NNODES}_${NP}_${NTHREADS} --search-dir src:r=. -- ./${APP_EXE} ${APP_ARGS} : -np $1 ./${APP_EXE} ${APP_ARGS} > OUTPUT_ADVISOR_SURVEY_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}
#mpirun -n ${NP} amplxe-cl --collect general-exploration --result-dir vtune_survey_${NQUBITS}_${NNODES}_${NP}_${NTHREADS} --search-dir src:r=. -- ./${APP_EXE} ${APP_ARGS} : -np $1 ./${APP_EXE} ${APP_ARGS} > OUTPUT_ADVISOR_SURVEY_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}


#mpirun -n ${NP} advixe-cl -collect tripcounts -no-support-multi-isa-binaries -flops-and-masks -result-dir vtune_survey_${NQUBITS}_${NNODES}_${NP}_${NTHREADS} --search-dir src:r=. -- ./${APP_EXE} ${APP_ARGS} : -np ${NP} ./${APP_EXE} ${APP_ARGS} > OUTPUT_ADVISOR_TRIP_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}

mkdir itac_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}


mv ${APP_EXE}.single.stf ${APP_EXE}.prot itac_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}


echo "Finished"

mv slurm-${SLURM_JOBID}.out slurm_itac_${NQUBITS}_${NNODES}_${NP}_${NTHREADS}.out
