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
#if rank == 0:
#    sim.printStates("",[])

if sys.argv[2] == "1":
    sim.applyGateNCU1(sim.getGateX(), regD, target ,"X")
else:
    sim.applyGateNCU2(sim.getGateX(), regD, regA, target ,"X")

res1 = sim.applyMeasurementToRegister([target], True)

if rank == 0:
    print(list(regD) + [target], list(regA), res0, res1)
#    sim.printStates("",[])
