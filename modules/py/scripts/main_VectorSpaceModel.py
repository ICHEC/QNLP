"""
python ./scripts/main_VectorSpaceModel.py corpus.txt "l" False 16 1
"""

from PyQNLPSimulator import PyQNLPSimulator as p
import QNLP as q
import numpy as np

from itertools import product
import tempfile

import os

if __name__ == "__main__":

    if len(os.sys.argv) < 6:
        exit()

    CorpusPath = os.sys.argv[1]
    proc_mode = os.sys.argv[2] # l, s, None
    stop_words = os.sys.argv[3] # True, False
    num_basis_elems_n = os.sys.argv[4] # 2,3...
    num_basis_elems_v = os.sys.argv[5] # 2,3...

    # Load the corpus into the VectorSpaceModel class
    vsm = q.VectorSpaceModel.VectorSpaceModel(
        corpus_path=CorpusPath, 
        mode=proc_mode, 
        stop_words=stop_words
    )

    # Create the basis from the corpus
    basis = vsm.define_basis({'verbs' : num_basis_elems_v, 'nouns' : num_basis_elems_n})

    # Sort the basis elements by defining Hamiltonian path of dependencies
    vsm.sort_basis_tokens_by_dist("verbs", num_basis=num_basis_elems_v)
    vsm.sort_basis_tokens_by_dist("nouns", num_basis=num_basis_elems_n)

    # Assign gray-coded indexing for above sorted basis items
    vsm.assign_indexing("nouns")
    vsm.assign_indexing("verbs")

    # Define basis tokens encoding and decoding dicts
    encoding_dict = {"ns" : vsm.encoded_tokens["nouns"],
                    "v"  : vsm.encoded_tokens["verbs"],
                    "no" : vsm.encoded_tokens["nouns"]
                    }

    decoding_dict = {"ns" : { v:k for k,v in encoding_dict["ns"].items() },
                    "v"  : { v:k for k,v in encoding_dict["v"].items() },
                    "no" : { v:k for k,v in encoding_dict["no"].items() }
                    }

    # Register must be large enough to support 2*|nouns| + |verbs|
    len_no = int( np.ceil(np.log2( len(encoding_dict["no"]) )))
    len_v  = int( np.ceil(np.log2( len(encoding_dict["v"]) )))
    len_ns = int( np.ceil(np.log2( len(encoding_dict["ns"]) )))

    len_reg_memory = len_no + len_v + len_ns
    len_reg_ancilla = len_reg_memory + 2
    num_qubits = len_reg_memory + len_reg_ancilla


    print("""{}

    Requires {} qubits to encode data using {} 
    basis elements for V, {} for N, allowing a 
    maximum of {} unique patterns.

{}
    """.format("#"*48, num_qubits, num_basis_elems_v, num_basis_elems_n, (2**len_no) * (2**len_v) * (2**len_ns), "#"*48)
    )

    exit()

    """
    Require analysis of corpus to determine number of patterns to encode. The current 
    encoding method assumes unique patterns, though this can be extended to multiple 
    occurrences of the same patterns later. Additionally, we can store 2^num_qubits 
    unique patterns. As such, we will restrict the patterns to encode by limiting the
    space over which we analyse the corpus-to-basis mapping.
    """
    num_bin_pattern = 8

    text = """
    cats eat tuna.
    dogs eat everything.
    cats kill birds.
    people feed dogs.
    bakers use ingredients.
    """
    import nltk
    token_words = nltk.word_tokenize(text)
    nltk.pos_tag(token_words)

    fp = tempfile.NamedTemporaryFile(mode='w')
    fp.write(text);
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