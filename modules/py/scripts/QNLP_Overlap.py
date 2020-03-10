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
from QNLP.proc.HammingDistance import HammingDistance
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

if rank == 0:
    s_encoder = simple.SimpleEncoder(num_nouns=NUM_BASIS_NOUN, num_verbs=NUM_BASIS_VERB)
    assert (len(sys.argv) > 1)
    corpus_file=sys.argv[1] 
    if not os.path.isfile(corpus_file):
        print ("Error: Inputted file does not exist")
        sys.exit()
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

    for idx,sentence in enumerate(sentences):
        superpos_patterns = list( product( list(sentence[0].values())[0], list(sentence[1].values())[0], list(sentence[2].values())[0] ) )
        # Generate all combinations of the bit-patterns for superpos states
        for patt in superpos_patterns: 
            num = q.utils.encode_binary_pattern_direct(patt, encoding_dict)
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

#    from IPython import embed; embed()

else:
    reg_memory = None
    reg_aux = None
    len_reg_memory = None
    vec_to_encode = None
    shot_counter = None
    encoding_dict = None
    bit_shifts = None

reg_memory = comm.bcast(reg_memory, root=0)
reg_aux = comm.bcast(reg_aux, root=0)
vec_to_encode = comm.bcast(vec_to_encode, root=0)
shot_counter = comm.bcast(shot_counter, root=0)
encoding_dict = comm.bcast(encoding_dict, root=0)

num_qubits = len(reg_memory) + len(reg_aux)

#Explicitly disable fusion as it can cause incorrect results
use_fusion = False

if os.environ.get('RESOURCE_EST') is not None:
    print("Overriding default qubit count")
    num_qubits = 1

sim1 = p(num_qubits, use_fusion)
sim2 = p(num_qubits, use_fusion)
normalise = True

if rank == 0:
    try:
        num_exps = int(sys.argv[2])
    except Exception as e:
        num_exps = 10

else:
    num_exps = None

num_exps=comm.bcast(num_exps, root=0)

num_faults = 0

# Use all potential string patterns as test
test_strings = list(product(encoding_dict["ns"].values(), encoding_dict["v"].values(), encoding_dict["no"].values()))


for i_ts1,ts1 in enumerate(test_strings[:-1]):
    test_pattern1 = q.utils.encode_binary_pattern_direct(ts1, encoding_dict)
    if test_pattern1 in vec_to_encode:
        if rank == 0:
            print("Pattern {} already exists. Skipping".format(test_pattern1))
            sys.stdout.flush()
        continue

    for i_ts2,ts2 in enumerate(test_strings[i_ts1+1:]):
        test_pattern2 = q.utils.encode_binary_pattern_direct(ts2, encoding_dict)

        if test_pattern2 in vec_to_encode:
            if rank == 0:
                print("Pattern {} already exists. Skipping encoding of {} and {}".format(test_pattern2, test_pattern1, test_pattern2))
                sys.stdout.flush()
            continue

        sim1.initRegister()
        sim2.initRegister()

    # Encode
        sim1.encodeBinToSuperpos_unique(reg_memory, reg_aux, vec_to_encode, len(reg_memory))
        sim2.encodeBinToSuperpos_unique(reg_memory, reg_aux, vec_to_encode, len(reg_memory))

# Compute Hamming distance between test pattern and encoded patterns
        sim1.applyHammingDistanceRotY(test_pattern1, reg_memory, reg_aux, len(reg_memory))
        sim2.applyHammingDistanceRotY(test_pattern2, reg_memory, reg_aux, len(reg_memory))

    # Measure
        sim1.collapseToBasisZ(reg_aux[len(reg_aux)-2], 1)
        sim2.collapseToBasisZ(reg_aux[len(reg_aux)-2], 1)

        val = sim1.overlap(sim2)

        if rank==0:
            print("|<{}|{}>|^2,|<{}|{}>|^2,{}".format(test_pattern1, test_pattern2, test_string1, test_string2, np.abs(val)**2))
            sys.stdout.flush()

MPI.Finalize()
exit()
