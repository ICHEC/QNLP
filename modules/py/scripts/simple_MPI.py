#!/usr/bin/env python
# coding: utf-8

from mpi4py import MPI

from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np

num_qubits = 24

# Create simulator object

use_fusion = False

sim = p(num_qubits, use_fusion)
sim.initRegister()

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
val = 0

sim.applyGateX(0)
sim.applyGateH(2)
sim.applyGateH(4)
sim.applyGateX(7)
sim.applyGateX(22)

val = sim.applyMeasurementToRegister(range(num_qubits), True)
print("RANK={} VAL={}".format(rank,val))

comm.Barrier()

"""
# Note, performing operations on rank=0 only will causes failures. The following example will fail.
if rank == 0:
    sim.initRegister()
    val = sim.applyMeasurementToRegister(range(num_qubits), True)
    print(val)
"""
