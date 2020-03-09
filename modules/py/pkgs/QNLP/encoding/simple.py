from .encoder_base import EncoderBase

class SimpleEncoder(EncoderBase):
    """
    This cyclic encoder is the simplest (and currently the only used) encoding scheme to ensure 
    equal Hamming distances between items separated by n indices. Not very efficient in use of qubits,
    but simple enough to implement.
    """

    def __init__(self, num_nouns, num_verbs):
        assert(num_nouns%2 == 0)
        assert(num_verbs%2 == 0)
        self.num_nouns = num_nouns
        self.num_verbs = num_verbs
        self.enc_mapping_nouns = self._gen_encoding_vals(num_nouns)
        self.enc_mapping_verbs = self._gen_encoding_vals(num_verbs)
        self.dec_mapping_nouns = { v:k for (k,v) in self.enc_mapping_nouns.items() }
        self.dec_mapping_verbs = { v:k for (k,v) in self.enc_mapping_verbs.items() }

    def _gen_encoding_vals(self, num_items):
        ""
        num_shifts = num_items//2
        vals = [0]*num_items
        for i in range(1, num_shifts+1):
            vals[i] = vals[i-1] + (0b1 << (i-1))
        for i in range(1, num_shifts):
            vals[i + num_shifts ] = vals[i + num_shifts -1] - (0b1 << (i-1))
        val_dict = { k:v for (k,v) in zip(range(len(vals)), vals)}
        return val_dict

    def encode(self, bin_val, data):
        val_offset = bin_val
        if data == "nouns":
            enc_mapping = self.enc_mapping_nouns
        elif data == "verbs":
            enc_mapping = self.enc_mapping_verbs
        else:
            raise Exception("Unknown data set: {}".format(data))

        if val_offset not in enc_mapping:
            raise(ValueError("Encoding index value not within expected range. Value={}".format(val_offset)))
        return enc_mapping[val_offset]
    
    def decode(self, bin_val, data):
        val_offset = bin_val 
        if data == "nouns":
            dec_mapping = self.dec_mapping_nouns
        elif data == "verbs":
            dec_mapping = self.dec_mapping_verbs
        else:
            raise Exception("Unknown data set: {}".format(data))
        if val_offset not in self.dec_mapping:
            raise(ValueError("Decoding index value not within expected range. Value={}".format(val_offset)))
        return dec_mapping[val_offset]
