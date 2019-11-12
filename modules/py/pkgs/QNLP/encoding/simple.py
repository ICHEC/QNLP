from .encoder_base import EncoderBase
"""
"""
class SimpleEncoder(EncoderBase):
    """
    This encoder is the simplest (and currently the only implemented) encoding scheme to ensure 
    equal Hamming distances between items separated by n indices. Not very efficient in use of qubits,
    but simple enough to implement.
    """

    def __init__(self, num_qubits=4):
        self.enc_mapping = self._gen_encoding_vals(num_qubits)
        self.dec_mapping =  { v:k for (k,v) in self.enc_mapping.items() }

    def _gen_encoding_vals(self, num_qubits):
        ""
        num_shifts = num_qubits
        vals = [0]*2*num_qubits
        for i in range(1,num_shifts+1):
            vals[i] = vals[i-1] + (0b1 << (i-1))
        for i in range(1, num_qubits):
            vals[i + num_qubits ] = vals[i + num_qubits -1] - (0b1 << (i-1))
        val_dict = { k:v for (k,v) in zip(range(len(vals)), vals)}
        return val_dict

    def encode(self, bin_val):
        val_offset = bin_val 
        if val_offset not in self.enc_mapping:
            raise(ValueError("Encoding index value not within expected range. Value={}".format(val_offset)))
        return self.enc_mapping[val_offset]
    
    def decode(self, bin_val):
        val_offset = bin_val 
        if val_offset not in self.dec_mapping:
            raise(ValueError("Decoding index value not within expected range. Value={}".format(val_offset)))
        return self.dec_mapping[val_offset]
