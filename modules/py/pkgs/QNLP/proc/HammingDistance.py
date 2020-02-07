from PyQNLPSimulator import DCMatrix as dcm
import numpy as np

class HammingDistance:
    def __init__(self, num_bits):
        self.num_bits = num_bits


    def _UMatrix(self, theta):
        npmat = np.array([[np.exp(1j*theta), 0], [0, 1]])
        return dcm(npmat.flatten())

    def _UMatrixPowMin2(self,theta):
        npmat = np.linalg.matrix_power([[np.exp(1j*theta), 0], [0, 1]], -2 )
        return dcm(npmat.flatten())

    def _step1(self, simulator, reg_mem, reg_aux, pattern):
        simulator.encodeToRegister(pattern, reg_aux[0:-2], len(reg_mem))

    def _step2(self, simulator, reg_mem, reg_aux):
        for i in range(len(reg_aux)-2):
            simulator.applyGateCX(reg_aux[i], reg_mem[i])
            simulator.applyGateX(reg_mem[i])

    def _step3(self, simulator, reg_mem, reg_aux):
        theta = np.pi / (2.0 * len(reg_mem))
    
        UMat = UMatrix(theta)
        CUMatPowMin2 = UMatrixPowMin2(theta)
    
        for j in range(len(reg_mem)):
            simulator.applyGateU(UMat, reg_mem[j], "U")
        for i in range(len(reg_mem)):
            simulator.applyGateCU(CUMatPowMin2, reg_aux[-2], reg_mem[i], "U")

    def _step4(self, simulator, reg_mem, reg_aux):
        for i in range(len(reg_aux)-2):
            simulator.applyGateX(reg_mem[i])
            simulator.applyGateCX(reg_aux[i], reg_mem[i])

    def _step5(self, simulator, reg_aux):
        sim.collapseToBasisZ(reg_aux[-2], False)

    def encode_hamming(self, simulator, reg_mem, reg_aux, pattern):
        simulator.applyGateH(reg_aux[-2])
        step1(simulator, reg_mem, reg_aux, pattern)
        step2(simulator, reg_mem, reg_aux)
        step3(simulator, reg_mem, reg_aux)
        step4(simulator, reg_mem, reg_aux)
        simulator.applyGateH(reg_aux[-2])
        step5(simulator, reg_aux)
    
