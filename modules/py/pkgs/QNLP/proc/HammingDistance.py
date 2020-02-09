from PyQNLPSimulator import DCMatrix as dcm
import numpy as np

class HammingDistance:
    def __init__(self, num_bits, simulator):
        self.num_bits = num_bits
        self.sim = simulator

    def _UMatrix(self, theta):
        npmat = np.array([[np.exp(1j*theta), 0], [0, 1]])
        return dcm(npmat.flatten())

    def _UMatrixPowMin2(self,theta):
        npmat = np.linalg.matrix_power([[np.exp(1j*theta), 0], [0, 1]], -2 )
        return dcm(npmat.flatten())

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
    
