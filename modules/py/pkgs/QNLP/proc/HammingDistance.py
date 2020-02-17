"""
This set of classes offers an approach to calculating the Hamming distance and encoding
into the state phase using PYthon. Pybind11 wrapped C++ methods are used where sensible.
"""

from abc import ABC, abstractmethod

from PyQNLPSimulator import DCMatrix
from PyQNLPSimulator import GateOps
import numpy as np

class HammingDistance:
    """
    This class is used to encode the Hamming distance between two 
    qubit registers into the phase.
    """
    def __init__(self, num_bits, simulator):
        self.num_bits = num_bits
        self.sim = simulator

    def _UMatrix(self, theta):
        npmat = np.array([[np.exp(1j*theta), 0], [0, 1]])
        return DCMatrix(npmat.flatten())

    def _UMatrixPowMin2(self,theta):
        npmat = np.linalg.matrix_power([[np.exp(1j*theta), 0], [0, 1]], -2 )
        return DCMatrix(npmat.flatten())

    @abstractmethod
    def calc(self):
        pass

class HammingDistanceExpITheta(HammingDistance):
    """
    Default approach taken by Trugenberger, and others to encode
    Hamming distance into the state phase. Results can be skewed
    by high degeneracy in encoded patterns.
    """
    def __init__(self, num_bits, simulator):
        super().__init__(num_bits, simulator)

    def _step1(self, reg_mem, reg_aux, pattern):
        self.sim.encodeToRegister(pattern, reg_aux[0:-2], len(reg_mem))

    def _step2(self, reg_mem, reg_aux):
        for i in range(len(reg_aux)-2):
            self.sim.applyGateCX(reg_aux[i], reg_mem[i])
            self.sim.applyGateX(reg_mem[i])

    def _step3(self, reg_mem, reg_aux):
        theta = np.pi / (2.0 * len(reg_mem))
    
        UMat = self._UMatrix(theta)
        CUMatPowMin2 = self._UMatrixPowMin2(theta)
    
        for j in range(len(reg_mem)):
            self.sim.applyGateU(UMat, reg_mem[j], "U")
        for i in range(len(reg_mem)):
            self.sim.applyGateCU(CUMatPowMin2, reg_aux[-2], reg_mem[i], "U")

    def _step4(self, reg_mem, reg_aux):
        for i in range(len(reg_aux)-2):
            self.sim.applyGateX(reg_mem[i])
            self.sim.applyGateCX(reg_aux[i], reg_mem[i])

    def _step5(self, reg_aux):
        self.sim.collapseToBasisZ(reg_aux[-2], False)

    def encode_hamming(self, reg_mem, reg_aux, pattern):
        self.sim.applyGateH(reg_aux[-2])
        self._step1(reg_mem, reg_aux, pattern)
        self._step2(reg_mem, reg_aux)
        self._step3(reg_mem, reg_aux)
        self._step4(reg_mem, reg_aux)
        self.sim.applyGateH(reg_aux[-2])
        self._step5(reg_aux)

    def hamming_aux_overwrite(self, reg_mem, reg_aux):
        for i in range(len(reg_mem)):
            self.sim.applyGateX(reg_aux[i])
            self.sim.applyGateCCX(reg_mem[i], reg_aux[i], reg_aux[-1])
            self.sim.applyGateX(reg_aux[i])
            self.sim.applyGateCSwap(reg_mem[i], reg_aux[i], reg_aux[-1])

    def calc(self, reg_mem, reg_aux, pattern):
        self.encode_hamming(reg_mem, reg_aux, pattern)

class HammingDistanceOverwriteAux(HammingDistance):
    """
    Intermediate routine to overwrite the data in the aux register
    with its Hamming dsiatnce to the data in the memory regsiter.
    """
    def __init__(self, num_bits, simulator):
        super().__init__(num_bits,simulator)

    def _encodePattern(self, reg_mem, reg_aux, pattern):
        self.sim.encodeToRegister(pattern, reg_aux[0:-2], len(reg_mem))

    def _overwriteAux(self, reg_mem, reg_aux, pattern):
        for idx, val in enumerate(reg_mem):
            self.sim.applyGateX(reg_aux[idx])
            self.sim.applyGateCCX(reg_mem[idx], reg_aux[idx], reg_aux[-2])
            self.sim.applyGateX(reg_aux[idx])
            self.sim.applyGateCSwap(reg_mem[idx], reg_aux[idx], reg_aux[-2])

    def calc(self, reg_mem, reg_aux, pattern):
        self._encodePattern(reg_mem, reg_aux, pattern)
        self._overwriteAux(reg_mem, reg_aux, pattern)
    
class HammingDistanceGroupRotate(HammingDistanceOverwriteAux):
    """
    Class attempt alternative for state amplitude weighting by
    Hamming distance. Uses oracle-based pattern, applying the 
    appropriate rotation angle to the state determined by matching
    the number of set bits to a pre-defined set of values.
    """

    def __init__(self, num_bits, simulator):
        super().__init__(num_bits, simulator)
        self.gops = GateOps(simulator)
        self.val_range = np.arange(-1.0, 1+1/self.num_bits, 2/num_bits)

    def calc(self, reg_mem, reg_aux, pattern):
        self._encodePattern(reg_mem, reg_aux, pattern)
        self._overwriteAux(reg_mem, reg_aux, pattern)
        self.sim.groupQubits(reg_aux, False)
        omap = self._oracle_angle_map()
        for k,v in omap.items():
            self.sim.addUToCache(v, "RY_{}".format(k))
            self.sim.applyOracleU(k, self._oracle_angle_map()[k], reg_aux[0:-2], reg_aux[-1], "RY_{}".format(k))

    def _angle_matrices(self):
        vals = []
        for i in self.val_range:
            vals.append( DCMatrix( np.asarray( self.gops.RY(np.arccos(i)) ).flatten() ) )
            
        return vals
    
    def _oracle_angle_map(self):
        val_map = {}
        mats = self._angle_matrices()
        for i in range(0, self.num_bits+1):
            val_map.update({ 2**i - 1 : mats[i]})
        return val_map
    
    def _set_bits_to_pattern(vals):
        if isinstance(vals, int):
            return (2**vals) - 1
        elif isinstance(vals, list):
            return [(2**val) -1 for val in vals]
        else:
            return None
