name = "PyQNLPSimulator"
from mpi4py import MPI #Required to ensure initialisation of MPI environment
from ._PyQNLPSimulator import *

from scipy.linalg import expm
import numpy as np
from itertools import product
from scipy.optimize import least_squares

class GateOps:
    """
    This class is primarily for experimenting with gate operations during algorithm development.
    This wraps the underlying gate types from the given simulator, and also gives a numpy version.

    THIS IS NOT TO BE USED FOR PERFORMANT/PRODUCTION CODE AND IS STRICTLY FOR DEV/TEST.
    """
    def __init__(self, simulator):
        self.gates= {
            "X" : simulator.getGateX(),
            "Y" : simulator.getGateX(),
            "Z" : simulator.getGateX(),
            "H" : simulator.getGateX(),
            "I" : simulator.getGateX(),
            "Xnp" : simulator.getGateX(),
            "Xnp" : simulator.getGateX(),
            "Xnp" : simulator.getGateX(),
            "Xnp" : simulator.getGateX(),
        }
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

    def RZPh(self, theta):
        "RZ excluding global phase of exp(iTheta/2)"
        #np.matrix(np.exp(0.5*1j*theta)*expm(-0.5*1j*theta*self.Znp)))
        return np.matrix([[1.0,0.0], [0., np.exp(1j*theta)] ])

    def RU(self, theta, U):
        res = None
        if isinstance(U, list):
            res = self.createU(U)
        elif isinstance(U, DCMatrix):
            res = U.as_numpy()
        else:
            raise TypeError("U is not an accepted type: should be list or DCMatrix")

        return np.matrix(expm(-0.5*1j*theta*res))

class UnitaryFinder:
    def __init__(self, simulator, unitary):
        from scipy.optimize import least_squares as lsq
        self.ls = lsq
        self.minFunc = None
        self.unitary = unitary
        self.unitarynp = None
        self.gates = ["X", "Y", "Z", "H", "RX", "RY", "RZ", "RZPh"]
        self.gate_ops = GateOps(simulator)
   
    def findOps(self, gate_order, cost_tol = 1e-3):
        lambda x: np.array([2*x[0] + x[1] - 1])
        num_params = 0
        for i in gate_order:
            if "R" in i[0]:
                num_params += 1

        def setMinFunc(x, gates):
            mm = self.gate_ops.createU(self.unitary).as_numpy()
            
            nn = 1
            param_val = 0
            for g in reversed(gates):
                if "R" in g[0]:
                    nn *= getattr(self.gate_ops , g)(x[param_val])
                    param_val += 1
                else:
                    nn *= getattr(self.gate_ops, "".join((g,"np")))
            nn = nn.tolist()
                        
            ml = [i for sublist in mm for i in sublist]
            mr = [np.real(i) for i in ml]
            mi = [np.imag(i) for i in ml]
            m = [] + mr + mi
            nl = [i for sublist in nn for i in sublist]
            nr = [np.real(i) for i in nl]
            ni = [np.imag(i) for i in nl]
            n = [] + nr + ni
            
            ab = np.sum([i**2 for i in np.subtract(m,n)])
            
            return ab
        
        if num_params > 0:
            res = least_squares(setMinFunc, [0]*num_params, args=(gate_order,), bounds=(-np.pi, np.pi))
            if res.cost <= cost_tol:
                return res.success, res.x
            else:
                return False, []
            
        else:
            rr = setMinFunc([], gate_order)
            return np.isclose(rr, 0, rtol=cost_tol), []

    def genOps(self, gates : list, depth=5):
        """Generate the combinations of operations.
        Same gate is never called twice in a row."""

        ops_list0 = list(product(gates, repeat=depth))
        ops_list = list(product(gates, repeat=depth))
        del_idx = set()
        for idx_ops_list, ops in enumerate(ops_list):
            for idx_ops in range(1,len(ops)):
                if ops[idx_ops] == ops[idx_ops-1]:
                    #del ops_list[idx_ops_list]
                    del_idx.add(idx_ops_list)
        ops_list = [val for idx,val in enumerate(ops_list) if idx not in del_idx]
        return ops_list