#!/bin/bash

# Preprocess the corpus using Python layer
yes n | python ${QNLP_ROOT}/modules/py/process_corpus.py ${QNLP_ROOT}/corpus/Ogden856.dat ${QNLP_ROOT}/corpus/jack_and_jill.txt l
python ${QNLP_ROOT}/modules/py/basis_check.py t

# Encode the state data and query using C++ layer
OMP_NUM_THREADS=1 ${QNLP_ROOT}/build/ISC_demo/ISC_demo