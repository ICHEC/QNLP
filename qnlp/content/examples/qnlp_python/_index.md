---
title: "QNLP example"
date: 2020-03-01T10:48:38Z
draft: false
---

## The following example is available as a Jupyter notebook titled `QNLP_Python.ipynb` in the source repository.


This notebook runs an example encoding of **"John rests inside, Mary walks outside"**. We initially define mappings for each of these tokens to a pre-defined set of basis tokens. These tokens are next represented as binary integers, and encoded using the QNLP runtime.

A test pattern is then constructed using **"adults stand inside"**, and the encoded patterns are queried and sampled to determine the closest encoded data, as learned from the given sentences.


The basis tokens are defined as:

$$
\begin{array}{c|c|c}
\mathbf{Dataset} & \mathbf{Word} & \mathbf{Bin.~Index} \newline
\newline
{n_s} & \mathrm{adult} & 00 \newline
{n_s} & \mathrm{child} & 01 \newline
{n_s} & \mathrm{smith} & 10 \newline
{n_s} & \mathrm{surgeon} & 11 \newline
\newline
{v} & \mathrm{stand} & 00 \newline
{v} & \mathrm{move} & 01 \newline
{v} & \mathrm{sit} & 10 \newline
{v} & \mathrm{sleep} & 11 \newline
\newline
{n_o} & \mathrm{inside} & 0 \newline
{n_o} & \mathrm{outside} & 1 \newline
\end{array}
$$

We define the mapping of *"John rests inside, Mary walks outside"* to this basis as:

$$
\begin{array}{c|c|c}
\mathbf{Dataset} & \mathbf{Word} & \mathbf{State} \newline
\newline
{n_s} & \mathrm{John} & (\vert 00 \rangle + \vert 10 \rangle)/\sqrt{2} \newline
{n_s} & \mathrm{Mary} & (\vert 01 \rangle + \vert 11 \rangle)/\sqrt{2} \newline
{v} & \mathrm{walk} & (\vert 00 \rangle + \vert 01 \rangle)/\sqrt{2} \newline
{v} & \mathrm{rest} & (\vert 10 \rangle + \vert 11 \rangle)/\sqrt{2} \newline
{n_o} & \mathrm{inside} & \vert 0 \rangle  \newline
{n_o} & \mathrm{outside} & \vert 1 \rangle  \newline
\end{array}
$$

Lastly, our test pattern is mapped onto the basis as **"adults stand inside"** $\rightarrow |00000\rangle$. The remainder of the notebook will encode the data using the given basis, encode the test pattern, and query the resulting register state using the Hamming distance to determine closeness.


```python
import os
os.environ["OMP_NUM_THREADS"] = "4"
```


```python
%matplotlib inline
from matplotlib import rc
rc('text', usetex=True)
import matplotlib.pyplot as plt
```

Import the C++ backed simulator package and the preprocessing Python package


```python
from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np
from itertools import product
from tqdm import tqdm
```

Predefine necessary data for encoding and calculate resources requirements


```python
# Define basis tokens encoding and decoding dicts
encoding_dict = {"ns" : { "adult":0b00, "child":0b11, "smith":0b10, "surgeon":0b01 },
                 "v"  : { "stand":0b00, "move":0b01, "sit":0b11, "sleep":0b10 },
                 "no" : { "inside":0b0,"outside":0b1 }
                }

decoding_dict = {"ns" : { 0b00:"adult", 0b11:"child", 0b10:"smith", 0b01:"surgeon"},
                 "v"  : { 0b00:"stand", 0b01:"move", 0b11:"sit", 0b10:"sleep"},
                 "no" : { 0b0:"inside", 0b1:"outside"}
                }
```


```python
len_reg_memory = 5
len_reg_auxiliary = len_reg_memory + 2
num_qubits = len_reg_memory + len_reg_auxiliary
num_bin_pattern = 8

test_pattern = 0
```

Create simulator object


```python
use_fusion = False
sim = p(num_qubits, use_fusion)
num_exps = 5000
normalise = True
```


```python
# Set up registers to store indices
reg_memory = [0]*len_reg_memory;
for i in range(len_reg_memory):
    reg_memory[i] = i + len_reg_auxiliary

reg_auxiliary = [0]*len_reg_auxiliary
for i in range(len_reg_auxiliary-2):
    reg_auxiliary[i] = i + 2
reg_auxiliary[-2] = 0
reg_auxiliary[-2] = 1
```


```python
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
sentences
```




    [[{'john': [0, 2]}, {'rest': [3, 2]}, {'inside': [0]}],
     [{'mary': [3, 1]}, {'walk': [0, 1]}, {'outside': [1]}]]




```python
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
        
vec_to_encode
```




    [6, 22, 4, 20, 25, 9, 27, 11]




```python
# Init result counter
count_even = {}
for i in vec_to_encode:
    count_even.update({i : 0})
```


```python
%%time
# Test of equally encoded bit patterns
for exp in tqdm(range(num_exps)):

    sim.initRegister()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory)
    
    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    count_even[val] += 1;

```

    100%|██████████| 5000/5000 [00:36<00:00, 136.10it/s]

    CPU times: user 2min 25s, sys: 696 ms, total: 2min 26s
    Wall time: 36.7 s


    



```python
# Test value to compare
test_pattern = 0; # 00000 ->  adult, stand, inside
#test_pattern = 7; # 00111 ->  adult, sleep, outside

# Init result counter
count = {}
for i in vec_to_encode:
    count.update({i : 0})
```


```python
# Repeated shots of experiment
for exp in tqdm(range(num_exps)):

    sim.initRegister()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_auxiliary, vec_to_encode, len_reg_memory)

    # Compute Hamming distance between test pattern and encoded patterns
    sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_auxiliary, len_reg_memory)

    # Measure
    sim.collapseToBasisZ(reg_auxiliary[len_reg_auxiliary-2], 1)
    
    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    count[val] += 1;

```

    100%|██████████| 5000/5000 [00:40<00:00, 124.57it/s]



```python
xlab_str = [",".join(q.utils.bin_to_sentence(i, encoding_dict, decoding_dict))  for i in list(count.keys())]
xlab_bin = ["{0:0{num_bits}b}".format(i, num_bits=len_reg_memory) for i in list(count.keys())]
[i[0]+"\n"+i[1] for i in zip(xlab_str,xlab_bin)]

# Sort histograms by P
hist_list_even = list(zip(
    [i[0]+r" $\vert$"+i[1]+r"$\rangle$" for i in zip(xlab_str,xlab_bin)],
    [i/np.sum(list(count_even.values())) for i in list(count_even.values())]
))
hist_list = list(zip(
    [i[0]+r" $\vert$"+i[1]+r"$\rangle$" for i in zip(xlab_str,xlab_bin)],
    [i/np.sum(list(count.values())) for i in list(count.values())]
))
#hist_list = sorted(hist_list, key=lambda pr: pr[1], reverse=True)
```


```python
labels = [x[0] for x in hist_list_even]
even_vals = [y[1] for y in hist_list_even]
post_vals = [y[1] for y in hist_list]

x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots()

rects1 = ax.bar(x - width/2, even_vals, width, label='Even superposition measurement')
rects2 = ax.bar(x + width/2, post_vals, width, label='Post-selection measurement')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel(r"$P(\textrm{{{}}})$".format("adult, sleep, outside"),fontsize=24)
ax.set_xticks(x)
ax.tick_params(axis='both', which='major', labelsize=16)
ax.set_xticklabels(labels, rotation=-30, ha="left",fontsize=16)
ax.legend(fontsize=16)

plt.axhline(y=1.0/len(count), color='crimson', linestyle="--")
plt.text(len(count)-0.1, 1.0/(len(count)), '$1/\sqrt{n}$', horizontalalignment='left', verticalalignment='center', fontsize=16)
plt.tight_layout()
fig.set_size_inches(20, 12, forward=True)
```


![png](/img/QNLP_Python.png)


As we can see, the closest discovered pattern using the given basis is "adult, sit, inside". 

