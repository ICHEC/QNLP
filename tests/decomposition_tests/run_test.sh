#!/bin/bash

#Load our pre-built qhipster library
module use /ichec/work/ichec001/modules/
module load qhipster

#Load required build toolchains
module load intel cmake3
mkdir build && cd build
CC=mpiicc CXX=mpiicpc cmake ..
make

srun -p DevQ -N 1 -A ichec001 -t 1:00:00 ./nqc_test
