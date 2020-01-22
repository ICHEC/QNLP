import ctypes
from .encoder_base import EncoderBase

"""
Adapated from example on https://en.wikipedia.org/wiki/Gray_code
"""
class GrayEncoder(EncoderBase):

    def encode(self, bin_val : ctypes.c_uint):
        return bin_val ^ ( bin_val >> 1)
    
    def decode(self, bin_val : ctypes.c_uint):
        bitmask = bin_val >> 1
        val = bin_val
        while bitmask != 0:
            val = val ^ bitmask
            bitmask = bitmask >> 1
        return val
