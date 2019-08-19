import ctypes
"""
Adapated from example on https://en.wikipedia.org/wiki/Gray_code
"""
class GrayCode:
    def __init__(self):
        pass

    def binToGray(self, bin_val : ctypes.c_uint):
        return bin_val ^ ( bin_val >> 1)
    
    def GrayToBin(self, bin_val : ctypes.c_uint):
        bitmask = bin_val >> 1
        val = bin_val
        while bitmask != 0:
            val = val ^ bitmask
            bitmask = bitmask >> 1
        return val
