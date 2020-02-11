#!/usr/bin/env python
# coding: utf-8

# # Calculating sentence similarity using a hybrid classical-quantum workflow
# 
# ### Lee J. O'Riordan and Myles Doyle
# 

# This notebook will serve to demonstrate a mixed hybrid workflow to examine 
# relationships between words, and respective sentence meanings using classical 
# and quantum information processing techniques.
# 
# Our goal is to analyse a corpus, extract key features rom it, and represent 
# those features in a quantum-compatible notation. We proceed to encide these 
# features into a quantum simulation environment using Intel-QS (formerly qHiPSTER),
# and query for similarities using the encoded quantum states. For this, we have 
# implemented various encoding and analysis strategies. Our primary method for 
# similarity uses a Hamming distance approach, wherein we apply a simplified 
# digital-to-analogue encoding strategy to allow the results to be obtained via
# measurement.

# We must load the Python QNLP packages & module (`QNLP`), and the C++-Python 
# bound simulator backend and algorithms (`PyQNLPSimulator`).

from mpi4py import MPI
from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np
from QNLP import DisCoCat
from QNLP.encoding import simple
from QNLP.proc.VerbGraph import VerbGraph as vg
import networkx as nx
from tqdm import tqdm
import sys
import os

from itertools import product
import tempfile

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
try:
    NUM_BASIS_NOUN = int(os.environ['NUM_BASIS_NOUN'])
    NUM_BASIS_VERB = int(os.environ['NUM_BASIS_VERB'])

    BASIS_NOUN_DIST_CUTOFF = int(os.environ['BASIS_NOUN_DIST_CUTOFF'])
    BASIS_VERB_DIST_CUTOFF = int(os.environ['BASIS_VERB_DIST_CUTOFF'])

    VERB_NOUN_DIST_CUTOFF = int(os.environ['VERB_NOUN_DIST_CUTOFF'])
    
except KeyError as e:    
##### Set defaults if the above env vars are not set.
    NUM_BASIS_NOUN = 2
    NUM_BASIS_VERB = 2
    BASIS_NOUN_DIST_CUTOFF = 2
    BASIS_VERB_DIST_CUTOFF = 2
    VERB_NOUN_DIST_CUTOFF = 2
#####

# Next, we load the corpus file using the vector-space model, defined in the 
# `VectorSpaceModel` class, specifying the mode of tagging, and whether to 
# filter out stop-words. For this notebook I have used the Project Gutenberg 
# [https://www.gutenberg.org/] edition of `Alice in Wonderland`, with simple 
# replacements to avoid incorrect tagging of elements (mostly standardising 
# apostrophes to \' and quotations to \"). 

#corpus_file = "/Users/mlxd/Desktop/qs_dev/intel-qnlp/corpus/11-0.txt"
if rank == 0:
    s_encoder = simple.SimpleEncoder(num_nouns=NUM_BASIS_NOUN, num_verbs=NUM_BASIS_VERB)
    assert (len(sys.argv) > 1)
    #corpus_file = "/ichec/work/ichec001/loriordan_scratch/intel-qnlp-python/11-0.txt"
    corpus_file=sys.argv[1] #"/ichec/home/staff/loriordan/woo.txt" #"/ichec/work/ichec001/loriordan_scratch/intel-qnlp-iqs2/joyce.txt"
    vsm = q.VectorSpaceModel.VectorSpaceModel(
        corpus_path=corpus_file,
        mode="l", 
        stop_words=True,
        encoder = s_encoder,
        use_spacy=True
    )

# From here we can specify the number of basis elements by occurrence in the 
# corpus. This will take the `num_basis_elems` most frequently occurring tokens
# in both verb and noun spaces respectively.
    num_basis_elems = 8
    basis     = vsm.define_basis({'verbs' : NUM_BASIS_VERB, 'nouns' : NUM_BASIS_NOUN})

# Next, we aim to sort the mapping of the basis tokens to a binary 
# representation for state encoding. By building a graph, and aiming to solve 
# the Hamiltonian cycle problem, we can ensure that the tokens are ordered such that any 
# nearest-neighbours have the shortest paths. This becomes necessary later when
# encoding information, as any incorrect bit-flips (potentially 
# offered by noise), should still allow some similarity to be represented. By 
# ordering the tokens by their minimum path lengths we may maintain closeness 
# in the presence of errors.

    verb_dist = vsm.sort_basis_tokens_by_dist("verbs", num_basis=NUM_BASIS_VERB)
    noun_dist = vsm.sort_basis_tokens_by_dist("nouns", num_basis=NUM_BASIS_NOUN)

# We now take the previously defined basis elements, and attempt to arrange 
# them such that the distance between nearest-neighbours in the corpus is 
# minimised. With this, we assign a Gray code value to the basis. This allows 
# us to ensure values closer together in the corpus have a shorter Hamming 
# distance between them. We make use of the DisCo-inspired compositional model,
# wherein the distances between words dictates their closeness; we use these 
# distances to define the edge-weights between nodes (tokens), and hence by 
# defining the problem as a TSP, we can find an ordering that ensures Hamming 
# distance relates directly to closeness of words.

    vsm.assign_indexing("nouns");
    vsm.assign_indexing("verbs");


# With the basis tokens correctly ordered, we may now map the other respective 
# elements in their space onto these bases. We aim to map the nouns onto the 
# noun basis, and likewise for the verbs. This allows us to represent any other
# word in the corpus in the given basis. We may encode arbitrary superposition 
# states to represent the tokens, which can be ascribed as a vector-space 
# representation model using quantum states.
# 
# For this, we begin by creating a `DisCoCat` object, and use to perform the 
# mappings of basis tokens.

    dcc = DisCoCat.DisCoCat()
    mapping_verbs = dcc.map_to_basis(vsm.tokens['verbs'] , verb_dist['verbs'], basis_dist_cutoff=BASIS_VERB_DIST_CUTOFF)
    mapping_nouns = dcc.map_to_basis(vsm.tokens['nouns'] , noun_dist['nouns'], basis_dist_cutoff=BASIS_NOUN_DIST_CUTOFF)

# For the above data, the meanings of the composite nouns `hall` and `table` can be represented as:
# 
# $$\begin{array}{ll}
# \vert \textrm{hall} \rangle &= a_0\vert \textrm{round} \rangle + a_1\vert \textrm{Rabbit} \rangle + a_2\vert \textrm{head} \rangle + a_3\vert \textrm{way} \rangle + a_4\vert \textrm{time} \rangle + a_5\vert \textrm{Alice} \rangle, \\
# \vert \textrm{table} \rangle &= b_{0}\vert \textrm{March} \rangle  + b_{1}\vert \textrm{tone} \rangle  + b_{2}\vert \textrm{round} \rangle  + b_{3}\vert \textrm{nothing} \rangle  + b_{4}\vert \textrm{Hare} \rangle  + b_{5}\vert \textrm{things} \rangle  + b_{6}\vert \textrm{thing} \rangle  + b_{7}\vert \textrm{way} \rangle  + b_{8}\vert \textrm{King} \rangle  + b_{9}\vert \textrm{time} \rangle  + b_{10}\vert \textrm{Alice} \rangle,
# \end{array}
# $$
# 
# where we assume each item in the basis set has an orthogonal quantum state 
# representing it. For the given 32-numbered bases, we can assume the 
# coefficient of any unlisted state is represented as zero.

# With nouns mappable to the noun basis set, and verbs mappable to the verbs 
# basis set, we may now examine the composite noun-verb-noun sentence 
# structures. This involves a similar approach to compositional mapping of 
# token to token-basis, wherein composite words close together can be 
# considered within the same NVN sentnence, and thus present themselves as a 
# constructable entity for binary-string quantum state encoding.
# 
# The following pair-wise relations are required for a fully generalised solution:
# 
# $$
# \begin{equation*}
# \begin{array}{l|c|c|c}
# & \textbf{Dataset 1} & \textbf{Dataset 2}  & \textbf{Use} \\
# \hline
# 1.&\textrm{noun basis} & \textrm{noun basis} & \textrm{noun basis binary ordering} \\
# 2.&\textrm{noun composite} & \textrm{noun basis} & \textrm{noun representative meaning} \\
# 3.&\textrm{noun composite} & \textrm{noun composite} & \textrm{inter-noun relationships} \\
# \hline
# 4.&\textrm{verb basis} & \textrm{verb basis} & \textrm{verb basis binary ordering} \\
# 5.&\textrm{verb composite} & \textrm{verb basis} & \textrm{verb representative meaning} \\
# 6.&\textrm{verb composite} & \textrm{verb compsite} & \textrm{inter-verb relationships} \\
# \hline
# 7.&\textrm{noun basis} & \textrm{verb basis} & \textrm{compiled bit-strings for encoding} \\
# 8.&\textrm{noun composite} & \textrm{verb composite} & \textrm{compositional meaning for bit-string generation} \\
# \end{array}
# \end{equation*}
# $$

# Additionally, one may add a more complex parameter exploration space by 
# customising the tagging options; here the use of lemmatisation `(mode="l")`, 
# stemming `(mode="s")`, or default tagging options `(mode=None)`, results in 
# a different set of words in the above graphs.

# From here, we define our encoding and decoding dictionaries.

# Define basis tokens encoding and decoding dicts
    encoding_dict = {"ns" : vsm.encoded_tokens["nouns"],
                     "v"  : vsm.encoded_tokens["verbs"],
                     "no" : vsm.encoded_tokens["nouns"]
                    }

    decoding_dict = {"ns" : { v:k for k,v in encoding_dict["ns"].items() },
                      "v"  : { v:k for k,v in encoding_dict["v"].items() },
                     "no" : { v:k for k,v in encoding_dict["no"].items() }
                    }

# With the above information, we can now determine the required resources to 
# store our data in a qubit register.

# Register must be large enough to support 2*|nouns| + |verbs| in given encoding
    len_reg_memory =    q.encoding.utils.pow2bits( int(np.max( list(encoding_dict['v'].values()))) )[1] + \
                        q.encoding.utils.pow2bits( int(np.max( list(encoding_dict['no'].values()))) )[1] + \
                        q.encoding.utils.pow2bits( int(np.max( list(encoding_dict['ns'].values()))) )[1] 

    len_reg_aux = len_reg_memory + 2
    num_qubits = len_reg_memory + len_reg_aux

    print("""{}
Requires {} qubits to encode data using {} 
noun and {} verb basis elements, allowing a 
maximum of {} unique patterns.
{}
""".format("#"*48, num_qubits, NUM_BASIS_NOUN, NUM_BASIS_VERB, (NUM_BASIS_NOUN**2)*NUM_BASIS_VERB, "#"*48)
    )

# Using encoded bitstrings for bases, look-up mapping terms for composite nouns
# and verbs, create bitstrings and generate quantum states.

    ns = []

    verb_bits = int(np.log2(len(verb_dist['verbs'])))
    noun_bits = int(np.log2(len(noun_dist['nouns'])))

    bit_shifts = [i[1] for i in q.utils.get_type_offsets(encoding_dict)]
    bit_shifts.reverse()

    bit_shifts.insert(0,0)
    bit_shifts = np.cumsum(bit_shifts)
    bit_shifts

    corpus_list_n = vsm.tokens['nouns']
    corpus_list_v = vsm.tokens['verbs']
    dist_cutoff = BASIS_VERB_DIST_CUTOFF

    v_list = vg.calc_verb_noun_pairings(corpus_list_v, corpus_list_n, VERB_NOUN_DIST_CUTOFF)

    sentences = []

    for v in v_list:
        for i in v.lr_nouns.keys(): #product(v.left_nouns, [v.verb], v.right_nouns):
        #ns,s,no = mapping_nouns[i[0]], mapping_verbs[i[1]], mapping_nouns[i[2]]
        #if v.left_nouns != None and v.right_nouns != None:
            if mapping_nouns[i[0]] != None and mapping_verbs[v.verb] != None and mapping_nouns[i[1]] != None:
                sentences.append(
                    [   {i[0] : [encoding_dict['ns'][k] for k in mapping_nouns[i[0]].keys()] },
                        {v.verb : [encoding_dict['v'][k] for k in mapping_verbs[v.verb].keys()] },
                        {i[1] : [encoding_dict['no'][k] for k in mapping_nouns[i[1]].keys()] }
                    ]
                )
    sentences
    print("Sentences matching noun-verb-noun structure captured as:", sentences)

    # Set up registers to store indices
    # Keeping aux register and control registers in these positions
    # to reduce overhead during encoding stages. ~25% faster than data-aux-control
    reg_memory = [0]*len_reg_memory;
    for i in range(len_reg_memory):
        reg_memory[i] = i + len_reg_aux

    reg_aux = [0]*len_reg_aux
    for i in range(len_reg_aux-2):
        reg_aux[i] = i + 2
    reg_aux[-2] = 0
    reg_aux[-1] = 1
    print("REG_MEM=",reg_memory)
    print("REG_AUX=",reg_aux)
    
#Create list for the patterns to be encoded
    vec_to_encode = []

# Generate bit-patterns from sentences and store in vec_to_encode

    for idx in range(len(sentences)):
        superpos_patterns = [list(sentences[idx][i].values())[0] for i in range(3)]
        # Generate all combinations of the bit-patterns for superpos states
        for patt in list(product(superpos_patterns[2], superpos_patterns[1], superpos_patterns[0])):
            num = 0
            for val in zip(patt, bit_shifts):
                num += (val[0] << val[1])
            vec_to_encode.extend([num])

#Need to remove duplicates        
    vec_to_encode = list(set(vec_to_encode))
    vec_to_encode.sort()


    d ={"sentences" : len(sentences),
        "patterns" : len(vec_to_encode),
        "NUM_BASIS_NOUN" : NUM_BASIS_NOUN,
        "NUM_BASIS_VERB" : NUM_BASIS_VERB,
        "BASIS_NOUN_DIST_CUTOFF" : BASIS_NOUN_DIST_CUTOFF,
        "BASIS_VERB_DIST_CUTOFF" : BASIS_VERB_DIST_CUTOFF,
        "VERB_NOUN_DIST_CUTOFF" : VERB_NOUN_DIST_CUTOFF 
    }
    print(d)
    print("Encoding data:", vec_to_encode)

    # Counter for experiment results; key exists for each potentially unique outcome defined by encoding
    shot_counter = {}

    for i in vec_to_encode:
        shot_counter.update({i : 0})

else:
    reg_memory = None
    reg_aux = None
    len_reg_memory = None
    vec_to_encode = None
    shot_counter = None

reg_memory = comm.bcast(reg_memory, root=0)
reg_aux = comm.bcast(reg_aux, root=0)
vec_to_encode = comm.bcast(vec_to_encode, root=0)
shot_counter = comm.bcast(shot_counter, root=0)

num_qubits = len(reg_memory) + len(reg_aux)

#Explicitly disable fusion as it can cause incorrect results
use_fusion = False

sim = p(num_qubits, use_fusion)
normalise = True

if rank == 0:
    try:
        num_exps = int(sys.argv[2])
    except Exception as e:
        num_exps = 10

    pbar = tqdm(total=num_exps)
    pbar.update(0)
else:
    num_exps = None

num_exps=comm.bcast(num_exps, root=0)

test_pattern = 0
num_faults = 0

if rank == 0:
    test_string = (encoding_dict['ns']['hatter'], encoding_dict['v']['say'], encoding_dict['no']['queen'])
    for val in zip(test_string, bit_shifts):
        test_pattern += (val[0] << val[1])
    print("Test data: {};   pattern: {}".format(q.utils.bin_to_sentence(test_pattern, encoding_dict, decoding_dict), test_pattern))

for exp in range(num_exps):
    sim.initRegister()

    if rank == 0:
        print("Encoding {} patterns for experiment {} of {}".format(len(vec_to_encode), exp+1, num_exps))
        sys.stdout.flush()

    # Encode
    sim.encodeBinToSuperpos_unique(reg_memory, reg_aux, vec_to_encode, len(reg_memory))

    # Compute Hamming distance between test pattern and encoded patterns
    sim.applyHammingDistanceRotY(test_pattern, reg_memory, reg_aux, len(reg_memory))

    # Measure
    sim.collapseToBasisZ(reg_aux[len(reg_aux)-2], 1)

    val = sim.applyMeasurementToRegister(reg_memory, normalise)
    print(sim.getGateCounts())
    try:
        shot_counter[val] += 1
    except Exception as e:
        print("Measured pattern {} does not match data for experiment {} of {}. Discarding.".format(val, exp+1, num_exps), file=sys.stderr)
        sys.stderr.flush()
        num_faults += 1
        continue

    if rank == 0:
        pbar.update(1)
        print("Measured pattern {} for experiment {} of {}".format(val, exp+1, num_exps))
        print(pbar)
        sys.stdout.flush()

if rank == 0:
    pbar.close()
    print("#"*48, "Shot counter values")
    print(shot_counter)
    print ("Number of faults: {}".format(num_faults))

    key_order = list(shot_counter.keys())

    xlab_str = [",".join(q.utils.bin_to_sentence(i, encoding_dict, decoding_dict)) for i in key_order]
    xlab_bin = ["{0:0{num_bits}b}".format(i, num_bits=len_reg_memory) for i in key_order ]

    pattern_dict = {k:v for k,v in zip(xlab_str, xlab_bin)}
    pattern_count = {k:v for k,v in zip(xlab_str, [shot_counter[i] for i in key_order])}

    print("#"*48, "Shot counter values key to token state")
    print(pattern_dict)
    print("#"*48, "Token state counts")
    print(pattern_count)

    import matplotlib as mpl
    mpl.use('Agg')
    import matplotlib.pyplot as plt

    hist_list = list(zip(
        [i[0]+r" |"+i[1]+r">" for i in zip(xlab_str,xlab_bin)],
        [i/np.sum(list(shot_counter.values())) for i in list(shot_counter.values())]
    ))

    labels = [x[0] for x in hist_list]
    post_vals = [y[1] for y in hist_list]

    x = np.arange(len(labels))  # the label locations
    width = 0.65  # the width of the bars

    fig, ax = plt.subplots()

    rects2 = ax.bar(x + width/2, post_vals, width, label='Measurement')

    test_pattern_str = ",".join(q.utils.bin_to_sentence(test_pattern, encoding_dict, decoding_dict))
    print("Test pattern={}".format(test_pattern_str))

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel(r"P({})".format(test_pattern_str),fontsize=24)
    ax.set_xticks(x)
    ax.tick_params(axis='both', which='major', labelsize=10)
    ax.set_xticklabels(labels, rotation=-35, ha="left",fontsize=10)
    ax.legend(fontsize=16)

    plt.axhline(y=1.0/len(shot_counter), color='crimson', linestyle="--")
    plt.text(len(shot_counter)-0.1, 1.0/(len(shot_counter)), '1/sqrt(n)', horizontalalignment='left', verticalalignment='center', fontsize=16)
    plt.tight_layout()
    fig.set_size_inches(20, 12, forward=True)
    plt.savefig("qnlp_e2e.pdf")

    import  pickle
    f = open("qnlp_e2e_{}.pkl".format(test_pattern_str),"wb")
    pickle.dump(shot_counter, f)
    f.close()
