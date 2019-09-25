name = "PyQNLPSimulator"
from mpi4py import MPI #Required to ensure initialisation of MPI environment
from ._PyQNLPSimulator import *

from scipy.linalg import expm
import numpy as np

class GateOps:
    """
    This class is primarily for experimenting with gate operations during algorithm development.
    This wraps the underlying gate types from the given simulator, and also gives a numpy version.

    THIS IS NOT TO BE USED FOR PERFORMANT/PRODUCTION CODE AND IS STRICTLY FOR DEV/TEST.
    """
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