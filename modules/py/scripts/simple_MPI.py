#!/usr/bin/env python
# coding: utf-8

from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np

#print("I am rank " , rank)

num_qubits = 12

# Create simulator object

use_fusion = False

sim = p(num_qubits, use_fusion)
#print("Rank {} is used={}".format(sim.rank, sim.is_used))

#sim.initRegister()
val = sim.applyMeasurementToRegister(range(num_qubits), True)
print(val)
"""
if sim.rank == 0:
    sim.initRegister()
    val = sim.applyMeasurementToRegister(range(num_qubits), True)
    print(val)
"""
