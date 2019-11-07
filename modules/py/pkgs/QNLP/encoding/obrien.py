from .encoder_base import EncoderBase
"""
"""
class OBrienEncoder(EncoderBase):
    """
    O'Brien encoding scheme. Adapated from example on http://www.ahok.de/en/hoklas-code.html
    Triangle stepping encoding distances when excluding first and last bits.
    For simplicity, assumes 8 unique binary patterns (aka bases).
    For bitstrings of length 4, the encoding schemes is defined as:

    0 -> 0000
    1 -> 0001
    2 -> 0011
    3 -> 0010
    4 -> 0110
    5 -> 1110
    6 -> 1010
    7 -> 1011
    8 -> 1001
    9 -> 1000
    
    for a total of 10 unique patterns. If we ignore patterns 0000, and 1000 
    we can have unity incremented pairwise Hamming distances between 0001 
    and others up to 1110, and decremented thereafter. This leaves us with 8
    unique bitstrings. Assuming the graph-problem for basis ordering offers
    a Hamiltonian path, we can (possibly) use this pattern to better represent
    the distances between words.
    """

    def __init__(self):
        self.enc_mapping = {1 : 1, 2 : 3, 3 : 2, 4 : 6, 5 : 14, 6 : 10, 7 : 11, 8 : 9 }
        self.dec_mapping = { v:k for k,v in self.enc_mapping.items() }

    def encode(self, bin_val):
        val_offset = bin_val + 1
        if val_offset not in self.enc_mapping:
            raise(ValueError("Encoding index value not within expected range. Value={}".format(val_offset)))
        return self.enc_mapping[val_offset]
    
    def decode(self, bin_val):
        val_offset = bin_val + 1
        if val_offset not in self.dec_mapping:
            raise(ValueError("Decoding index value not within expected range. Value={}".format(val_offset)))
        return self.dec_mapping[val_offset]
