#!/usr/bin/env python

from matplotlib import rc
rc('text', usetex=True)
import matplotlib.pyplot as plt

# We must load the Python QNLP packages & module (`QNLP`), and the C++-Python bound simulator backend and algorithms (`PyQNLPSimulator`).

from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np

from itertools import product
import tempfile

# Next, we load the corpus file using the vector-space model, defined in the `VectorSpaceModel` class, specifying the mode of tagging, and whether to filter out stop-words.

vsm = q.VectorSpaceModel.VectorSpaceModel(
    corpus_path="/Users/mlxd/Desktop/qs_dev/intel-qnlp/corpus/84-0.txt", 
    mode='l', 
    stop_words=False
)

num_basis_elems = 32
basis     = vsm.define_basis({'verbs' : num_basis_elems, 'nouns' : num_basis_elems})

verb_dist = vsm.sort_basis_tokens_by_dist("verbs", num_basis=num_basis_elems)
noun_dist = vsm.sort_basis_tokens_by_dist("nouns", num_basis=num_basis_elems)

# We now take the previously defined basis elements, and attempt to arrange them such that the distance between nearest-neighbours in the corpus is minimised. With this, we assign a Gray code value to the basis. This allows us to ensure values closer together in the corpus have a shorter Hamming distance between them. We make use of the DisCo-inspired compositional model, wherein the distances between words dictates their closeness; we use these distances to define the edge-weights between nodes (tokens), and hence by defining the problem as a TSP, we can find an ordering that ensures Hamming distance relates directly to closeness of words.

vsm.assign_indexing("nouns")
vsm.assign_indexing("verbs")

#from IPython import embed; embed()
from QNLP import DisCoCat

dcc = DisCoCat.DisCoCat()
mapping_verbs = dcc.map_to_basis(vsm.tokens['verbs'] , noun_dist['verbs'], basis_dist_cutoff=num_basis_elems)
mapping_nouns = dcc.map_to_basis(vsm.tokens['nouns'] , noun_dist['nouns'], basis_dist_cutoff=num_basis_elems)

import networkx as nx
g_nouns = nx.graph.Graph()
g_verbs = nx.graph.Graph()

#Add basis words to graph
for v in noun_dist['verbs']:
    g_verbs.add_node(v)
for v in noun_dist['nouns']:
    g_nouns.add_node(v)

# Add mapped/composite words to graph
for i in ['dawn', 'torrent']:
    g_nouns.add_node(i)

# Set connections between basis and mapped words
    for b_words,b_weights in mapping_nouns[i].items():
        g_nouns.add_edge(i, b_words, weight=b_weights)

#Remove unconnected basis words
uncon = list(nx.isolates(g_nouns))
g_nouns.remove_nodes_from(uncon)

# create diagram positions and edge labels
pos = nx.spring_layout(g_nouns, scale=5.0, k=20/np.sqrt(g_nouns.order())) #nx.bipartite_layout(g_nouns, nx.bipartite.sets(g_nouns)[0], scale=5) #nx.shell_layout(g_nouns, iterations=500)
labels = nx.get_edge_attributes(g_nouns,'weight')

#Colourmap for the nodes
cmap = []
for n in g_nouns:
    if n in noun_dist['nouns']:
        cmap.append('teal')
    else:
        cmap.append('plum')

nx.draw(g_nouns, pos=pos, with_labels=True, node_size=1000, node_color=cmap, font_size=10)
plt.margins(0.1)
nx.draw_networkx_edge_labels(g_nouns, pos, edge_labels=labels, alpha=0.5)

#nx.draw(g_nouns, pos=pos, font_size=10, with_labels=True, node_size=2000, node_color=cmap)
plt.savefig("Composite_to_Basis_map_simple.pdf", bbox_inches="tight")

# From here, we define our encoding and decoding dictionaries.
exit()

# Define basis tokens encoding and decoding dicts
encoding_dict = {"ns" : vsm.encoded_tokens["nouns"],
                 "v"  : vsm.encoded_tokens["verbs"],
                 "no" : vsm.encoded_tokens["nouns"]
                }

decoding_dict = {"ns" : { v:k for k,v in encoding_dict["ns"].items() },
                 "v"  : { v:k for k,v in encoding_dict["v"].items() },
                 "no" : { v:k for k,v in encoding_dict["no"].items() }
                }

# With the above information, we can now determine the required resources to store our data in a qubit register.

# Register must be large enough to support 2*|nouns| + |verbs|
len_reg_memory = len(verb_dist) + 2*len(noun_dist)
len_reg_ancilla = len_reg_memory + 2
num_qubits = len_reg_memory + len_reg_ancilla
print("""{}
Requires {} qubits to encode data using {} 
basis elements in each space, allowing a 
maximum of {} unique patterns.
{}
""".format("#"*48, num_qubits, num_basis_elems, 2**num_qubits, "#"*48)
)

"""
Require analysis of corpus to determine number of patterns to encode. The current 
encoding method assumes unique patterns, though this can be extended to multiple 
occurrences of the same patterns later. Additionally, we can store 2^num_qubits 
unique patterns. As such, we will restrict the patterns to encode by limiting the
space over which we analyse the corpus-to-basis mapping.
"""
num_bin_pattern = 8

text = """
cats eat tuna
dogs eat everything
cats kill birds
people feed dogs
bakers use ingredients
"""
import nltk
token_words = nltk.word_tokenize(text)
nltk.pos_tag(token_words)

fp = tempfile.NamedTemporaryFile(mode='w')
fp.write(text)
fp.flush()

vsm2 = q.VectorSpaceModel.VectorSpaceModel(
    corpus_path=fp.name, 
    mode=0, 
    stop_words=False
)
fp.close()

num_basis_elems2 = 2
basis2     = vsm2.define_basis({'verbs' : num_basis_elems, 'nouns' : num_basis_elems})

verb_dist2 = vsm2.sort_basis_tokens_by_dist("verbs")
noun_dist2 = vsm2.sort_basis_tokens_by_dist("nouns")

vsm2.assign_indexing("nouns");
vsm2.assign_indexing("verbs")

from QNLP import DisCoCat

dcc = DisCoCat.DisCoCat()
mapping = dcc.map_to_basis(token_words, noun_dist2['verbs'], basis_dist_cutoff=3)

from IPython import embed; embed()

import networkx as nx
g = nx.graph.Graph()

for k,v in mapping.items():
    g.add_node(k)
for k,v in mapping.items():
    for k0,v0 in v.items():
        g.add_edge(k, k0)

nx.draw(g, pos=nx.spring_layout(g), with_labels=True)