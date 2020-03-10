# Utility functions in encoding and decoding of data
import numpy as np

def pow2bits(bin_val):
    """
    Convert the integer value to a tuple of the next power of two, 
    and number of bits required to represent it.
    """
    result = -1
    bits = (bin_val).bit_length()
    if bin_val > 1:
        result = 2**(bits)
    else:
        result = 1
        bits = 1
    return result, bits

def get_type_offsets(encoding_dict):
    l_ns = pow2bits( int(np.max( list(encoding_dict['ns'].values()))) )
    l_v  = pow2bits( int(np.max( list(encoding_dict['v'].values()))) )
    l_no = pow2bits( int(np.max( list(encoding_dict['no'].values()))) )
    return l_ns, l_v, l_no

def get_type_masks(encoding_dict, type_offsets=None):
    # Create the bit_masks of the required length to decode the value from bin_val
    if type_offsets == None:
        l_ns, l_v, l_no = get_type_offsets(encoding_dict)
    else:
        l_ns, l_v, l_no = type_offsets

    bitmask_ns = (l_ns[0]-1) << (l_v[1] + l_no[1])
    bitmask_v = (l_v[0]-1) << l_no[1]
    bitmask_no = (l_no[0]-1) 
    
    return bitmask_ns, bitmask_v, bitmask_no


def bin_to_sentence(bin_val, encoding_dict, decoding_dict, type_offsets=None):
    """
    Map the integer binary value to the result from the encoded basis mappings.
    """
    if type_offsets == None:
        l_ns, l_v, l_no = get_type_offsets(encoding_dict)
    else:
        l_ns, l_v, l_no = type_offsets
    
    bitmask_ns, bitmask_v, bitmask_no = get_type_masks(encoding_dict, type_offsets=(l_ns, l_v, l_no) )

    ns_val = (bin_val & bitmask_ns) >> (l_v[1] + l_no[1])
    v_val  = (bin_val & bitmask_v) >> l_no[1]
    no_val = (bin_val & bitmask_no)

    return decoding_dict["ns"][ns_val], decoding_dict["v"][v_val], decoding_dict["no"][no_val]


def gen_state_string(l):
    """Given a list of strings, generate the latex '\\vert {} \\rangle' representation of this in superposition."""
    result = ""
    str_template = r"\vert {} \rangle"
    for idx,i in enumerate(l):
        result += r"b_{{{}}}\vert \textrm{{{}}} \rangle ".format(idx, i[0])
        if i != l[-1]:
            result += " + "
    return result

def HammingInt(i1 : int, i2 : int):
    """Simple integer based hamming distance for bits"""
    return bin(i1 ^ i2).count("1")

def num_qubits(enc_dict : dict):
    """
    Assuming simple.py encoding and give qubits as num_tokens/2 per meaning space
    """
    
    num_ns = len(enc_dict['ns'])//2
    num_v = len(enc_dict['v'])//2
    num_no = len(enc_dict['no'])//2
    return num_ns, num_v, num_no

def encode_binary_pattern(pattern : (str,str,str), enc_dict : dict):
    """
    Pass in the pattern given by the basis token values as (ns,v,no)
    The pattern will be constructed into the appropriate binary string
    and output for encoding.
    """
    num_ns, num_v, num_no = num_qubits(enc_dict)
    result = enc_dict["ns"][pattern[0]] | \
                enc_dict["v"][pattern[1]] << num_ns | \
                enc_dict["no"][pattern[2]] << (num_ns + num_v)

    return result

def encode_binary_pattern_direct(pattern : [int,int,int], enc_dict : dict):
    """
    Pass in the pattern given by the basis token values as (ns,v,no)
    The pattern will be constructed into the appropriate binary string
    and output for encoding.
    """
    num_ns, num_v, num_no = num_qubits(enc_dict)
    result = pattern[0] | pattern[1] << num_ns | pattern[2] << (num_ns + num_v)

    return result


def decode_binary_pattern(pattern : int, dec_dict : dict):
    """
    Pass in the pattern given as the encoded integer.
    The pattern will be deconstructed into the appropriate token tuple
    and output for plotting/printing. Inverse of encode_binary_pattern.
    """
    num_ns, num_v, num_no = num_qubits(dec_dict)

    t_ns =   pattern & (2**num_ns -1)
    t_v  = ( pattern >> num_ns ) & (2**num_v -1)
    t_no = ( pattern >> (num_ns + num_v) ) & (2**num_no -1)
    
    return (dec_dict["ns"][t_ns], dec_dict["v"][t_v], dec_dict["no"][t_no])