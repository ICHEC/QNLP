from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np
from itertools import product
from tqdm import tqdm


# Import MPI4PY and set up MPI environment

from mpi4py import MPI
#assert MPI.COMM_WORLD.Get_size() > 1
rank = MPI.COMM_WORLD.Get_rank()

print("I am rank " , rank)


# Define basis tokens encoding and decoding dicts
encoding_dict = {"ns" : { "adult":0, "child":1, "smith":2, "surgeon":3 },
                 "v"  : { "stand":0, "move":1, "sit":2, "sleep":3 },
                 "no" : { "inside":0,"outside":1 }
                }

decoding_dict = {"ns" : { 0:"adult", 1:"child", 2:"smith", 3:"surgeon"},
                 "v"  : { 0:"stand", 1:"move", 2:"sit", 3:"sleep"},
                 "no" : { 0:"inside", 1:"outside"}
                }

len_reg_memory = 5
len_reg_auxiliary = len_reg_memory + 2
num_qubits = len_reg_memory + len_reg_auxiliary
num_bin_pattern = 8

test_pattern = 0


# Create simulator object

use_fusion = False
sim = p(num_qubits, use_fusion)
num_exps = 10000
normalise = True


# Set up registers to store indices
reg_memory = [0]*len_reg_memory;
for i in range(len_reg_memory):
    reg_memory[i] = i

reg_auxiliary = [0]*len_reg_auxiliary
for i in range(len_reg_auxiliary):
    reg_auxiliary[i] = i + len_reg_memory;


# data for encoding using the given basis
"John rests outside, Mary walks inside"
sentences = [
    [{"john" :   [encoding_dict["ns"]["adult"], encoding_dict["ns"]["smith"]]},
    {"rest" :    [encoding_dict["v"]["sit"], encoding_dict["v"]["sleep"]]},
    {"inside" :  [encoding_dict["no"]["inside"]] }],

    [{"mary" :   [encoding_dict["ns"]["child"], encoding_dict["ns"]["surgeon"]]},
    {"walk" :    [encoding_dict["v"]["stand"],  encoding_dict["v"]["move"]]},
    {"outside" :[encoding_dict["no"]["outside"]] }],
]
print(sentences)

# Create shift patterns for the encoding step
bit_shifts = [i[1] for i in q.utils.get_type_offsets(encoding_dict)]
bit_shifts.reverse()

bit_shifts.insert(0,0)
bit_shifts = np.cumsum(bit_shifts)

# Parse the sentences and generate the required bit-patterns
superpos_patterns = [list(sentences[0][i].values())[0] for i in range(3)]

#Create list for the patterns to be encoded
vec_to_encode = []

# Generate bit-patterns from sentences and store in vec_to_encode
for idx in range(len(sentences)):
    superpos_patterns = [list(sentences[idx][i].values())[0] for i in range(3)]
    # Generate all combinations of the bit-patterns for superpos states
    for i in list(product(superpos_patterns[2], superpos_patterns[1], superpos_patterns[0])):
        num = 0
        for val in zip(i,bit_shifts):
            num += (val[0] << val[1])
        vec_to_encode.extend([num])

print(vec_to_encode)

# Init result counter
count_even = {}
for i in vec_to_encode:
    count_even.update({i : 0})


# Test value to compare
#test_pattern = 0; # 00000 ->  adult, stand, inside
test_pattern = 7; # 00111 ->  adult, sleep, outside

# Init result counter
count = {}
for i in vec_to_encode:
    count.update({i : 0})

# Repeated shots of experiment
for exp in tqdm(range(num_exps)):

    sim.initRegister()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory)

    # Compute Hamming distance between test pattern and encoded patterns
    sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_auxiliary, len_reg_memory, num_bin_pattern)

    # Measure
    sim.collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1)

    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    if val in count:
        count[val] += 1;
    else:
        count[val] = 1


print("Experiments completed.")
print(count)

