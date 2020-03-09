"""
Shows the performance difference in optimised versus non optimised NCU operations.

mpirun -n 8 python ./ncu_opt_tester.py <num ctrl lines> <operating mode: {0,1}>

mpirun -n 8 python ./ncu_opt_tester.py 11 0 # Should be realtively fast
mpirun -n 8 python ./ncu_opt_tester.py 11 1 # Should take a while

"""

from PyQNLPSimulator import PyQNLPSimulator as p
import sys
from mpi4py import MPI

comm = MPI.COMM_WORLD
rank = comm.Get_rank()

num_ctrl = int(sys.argv[1])
num_qubits = num_ctrl*2 - 1

target = num_qubits-1

sim = p(num_qubits, False)
regD = range(num_ctrl)
regA = range(num_ctrl, target)

sim.initRegister()

for i in regD:
    if rank == 0:
        print("Applying gateX to {}".format(i))
    sim.applyGateX(i)

res0 = sim.applyMeasurementToRegister([target], True)

if sys.argv[2] == "1": #unoptimised
    sim.applyGateNCU(sim.getGateX(), regD, target ,"X")
else: #optimised
    sim.applyGateNCU(sim.getGateX(), regD, regA, target ,"X")

res1 = sim.applyMeasurementToRegister([target], True)

if rank == 0:
    print(list(regD) + [target], list(regA), "Before NCU={}".format(res0), "After NCU={}".format(res1))
