#!/usr/bin/env python
# coding: utf-8

from matplotlib import rc
#rc('text', usetex=True)
import matplotlib.pyplot as plt

# Import the C++ backed simulator package and the preprocessing Python package

from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np
from itertools import product

# Define basis tokens encoding and decoding dicts
encoding_dict = {"ns" : { "adult":0, "child":1, "smith":2, "surgeon":3, "artist":4, "scientist":5, "engineer":6, "chef":7 },
                 "v"  : { "stand":0, "move":1, "sit":2, "sleep":3 },
                 "no" : { "inside":0,"outside":1, "basement":2, "attic":3 }
                }

decoding_dict = { ktype:{vval:kval for kval,vval in vtype.items()} for ktype,vtype in encoding_dict.items() }

# Set the size of the required quantum register
len_reg_memory = 7
len_reg_auxiliary = len_reg_memory + 2
num_qubits = len_reg_memory + len_reg_auxiliary
num_bin_pattern = 34

# Create simulator object
use_fusion = False
sim = p(num_qubits, use_fusion)
num_exps = 1000
normalise = True

# Set up registers to store indices
reg_memory = [0]*len_reg_memory;
for i in range(len_reg_memory):
    reg_memory[i] = i

reg_auxiliary = [0]*len_reg_auxiliary
for i in range(len_reg_auxiliary):
    reg_auxiliary[i] = i + len_reg_memory;

# data for encoding using the given basis
"John rests upstairs, Mary walks (in the) cellar, James sits (in the) attic"
sentences = [
    [{"john" :   [encoding_dict["ns"]["adult"], encoding_dict["ns"]["smith"], encoding_dict["ns"]["artist"]]},
    {"rest" :    [encoding_dict["v"]["sit"], encoding_dict["v"]["sleep"]]},
    {"upstairs" :  [encoding_dict["no"]["inside"], encoding_dict["no"]["attic"]] }], #12

    [{"mary" :   [encoding_dict["ns"]["child"], encoding_dict["ns"]["surgeon"], encoding_dict["ns"]["scientist"], encoding_dict["ns"]["engineer"] ]},
    {"walk" :    [encoding_dict["v"]["stand"],  encoding_dict["v"]["move"]]},
    {"cellar" : [encoding_dict["no"]["outside"], encoding_dict["no"]["basement"]] }], #16 + 12 = 28
    
    [{"james" :  [encoding_dict["ns"]["adult"], encoding_dict["ns"]["engineer"], encoding_dict["ns"]["child"] ]},
    {"sits" :    [encoding_dict["v"]["sit"]]},
    {"doorway" : [encoding_dict["no"]["outside"], encoding_dict["no"]["inside"]] }], #6 +28 = 34
]
print("sentences = ", sentences)

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
        
vec_to_encode = list(set(vec_to_encode))

# Init result counter_even
count_even = {}
for i in vec_to_encode:
    count_even.update({i : 0})

for exper in range(num_exps):
    print("Initial encoding:=", exper)
    sim.initRegister()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory)
    
    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    count_even[val] += 1;

test_pattern = 38
pattern_string = q.utils.bin_to_sentence(test_pattern, encoding_dict, decoding_dict)
print("pattern_string=", pattern_string)

# Init result counter
count = {}
for i in vec_to_encode:
    count.update({i : 0})

# Repeated shots of experiment
for exper in range(num_exps):#, desc='Sim. experiment progress'):       
    print("Comparison:=", exper)

    sim.initRegister()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory)

    # Compute Hamming distance between test pattern and encoded patterns
    sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_auxiliary, len_reg_memory)

    # Measure
    sim.collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1)
    
    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    count[val] += 1

# Format the data to be shown in bar-plot

xlab_str = [",".join(q.utils.bin_to_sentence(i, encoding_dict, decoding_dict))  for i in list(count.keys())]
xlab_bin = ["{0:0{num_bits}b}".format(i, num_bits=len_reg_memory) for i in list(count.keys())]
[i[0]+"\n"+i[1] for i in zip(xlab_str,xlab_bin)]

# Sort histograms by P
hist_list_even = list(zip(
    [i[0]+" |"+i[1]+">" for i in zip(xlab_str,xlab_bin)],
    [i/np.sum(list(count_even.values())) for i in list(count_even.values())]
))
hist_list = list(zip(
    [i[0]+" |"+i[1]+">" for i in zip(xlab_str,xlab_bin)],
    [i/np.sum(list(count.values())) for i in list(count.values())]
))

# Plot the data for both the evenly encoded and post-selection measurements
labels = [x[0] for x in hist_list_even]
even_vals = [y[1] for y in hist_list_even]
post_vals = [y[1] for y in hist_list]

x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, even_vals, width, label='Even superposition measurement')
rects2 = ax.bar(x + width/2, post_vals, width, label='Post-selection measurement')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel("P({})".format(str(pattern_string).replace("'","")),fontsize=16)
ax.set_xticks(x)
ax.set_xticklabels(labels, rotation=-30, ha="left")
ax.legend()

plt.axhline(y=1/(num_exps/np.mean(list(count.values()))), color='crimson', linestyle="--")
plt.text(len(count)-0.1, 1.0/(len(count)), '1/sqrt(n)', horizontalalignment='left', verticalalignment='center')

plt.tight_layout()

plt.savefig("qnlp_vector_measurement.pdf")
print(count)

import pickle
with open('count.pickle', 'wb') as handle:
    pickle.dump(count, handle, protocol=pickle.HIGHEST_PROTOCOL)
