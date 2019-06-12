#!/bin/bash


# Preprocess the corpus using Python layer
pushd . > /dev/null

cd ${QNLP_ROOT}/modules/py
rm corpus.txt

echo "dogs like food\n cats hate dogs\n jack eats cake\n mary walked home\n bunnies drink water" > corpus.txt
#yes n | python ./process_corpus.py ${QNLP_ROOT}/corpus/Ogden856.dat ${QNLP_ROOT}/corpus/jack_and_jill.txt l
yes n | python ./process_corpus.py ${QNLP_ROOT}/corpus/Ogden856.dat corpus.txt l
python ${QNLP_ROOT}/modules/py/basis_check.py t

# Encode the state data and query using C++ layer
OMP_NUM_THREADS=1 ${QNLP_ROOT}/build/demos/isc_2019/encode_corpus -f ./qnlp_tagged_corpus.sqlite
popd > /dev/null
