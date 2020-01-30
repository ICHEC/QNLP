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
"""
def get_type_offsets(encoding_dict):
    l_ns = pow2bits( len(encoding_dict['ns']) -1 )
    l_v  = pow2bits( len(encoding_dict['v'])  -1 )
    l_no = pow2bits( len(encoding_dict['no']) -1 )
    return l_ns, l_v, l_no
"""
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

    no_val = (bin_val & bitmask_ns) >> (l_v[1] + l_ns[1])
    v_val  = (bin_val & bitmask_v) >> l_ns[1]
    ns_val = (bin_val & bitmask_no)
    
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
