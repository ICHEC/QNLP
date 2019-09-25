name = "PyQNLPSimulator"
from mpi4py import MPI
from ._PyQNLPSimulator import *

from scipy.linalg import expm
import numpy as np

"""
X = PyQNLPSimulator.getGateX()
Y = PyQNLPSimulator.getGateY()
Z = PyQNLPSimulator.getGateZ()
H = PyQNLPSimulator.getGateH()
I = PyQNLPSimulator.getGateI()

Xnp = np.matrix(X.as_numpy())
Ynp = np.matrix(Y.as_numpy())
Znp = np.matrix(Z.as_numpy())
Hnp = np.matrix(H.as_numpy())
Inp = np.matrix(I.as_numpy())
"""

class GateOps:
    def __init__(self, simulator):
        self.X = simulator.getGateX()
        self.Y = simulator.getGateY()
        self.Z = simulator.getGateZ()
        self.H = simulator.getGateH()
        self.I = simulator.getGateI()

        self.Xnp = np.matrix(self.X.as_numpy())
        self.Ynp = np.matrix(self.Y.as_numpy())
        self.Znp = np.matrix(self.Z.as_numpy())
        self.Hnp = np.matrix(self.H.as_numpy())
        self.Inp = np.matrix(self.I.as_numpy())

    def createU(self, vals : list):
        return DCMatrix(vals)

    def RX(self, theta):
        return np.matrix(expm(-0.5*1j*theta*self.Xnp))

    def RY(self, theta):
        return np.matrix(expm(-0.5*1j*theta*self.Ynp))

    def RZ(self, theta):
        return np.matrix(expm(-0.5*1j*theta*self.Znp))

    def RU(self, theta, U):
        res = None
        if isinstance(U, list):
            res = self.createU(U)
        elif isinstance(U, DCMatrix):
            res = U.as_numpy()
        else:
            raise TypeError("U is not an accepted type: should be list or DCMatrix")

        return np.matrix(expm(-0.5*1j*theta*res))