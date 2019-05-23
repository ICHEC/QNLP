###############################################################################

import sqlite3
import os
from typing import Dict, Tuple
import process_corpus as pc
import numpy as np

###############################################################################

class DisCoCat:
    """
    Implements precomputation for the DisCo(Cat) model to represent sentence meanings
    using category theory methods. See <PAPERS> for details.
    """
    def __init__(self,):
        pass

    def load_corpus(self, corpus_path):
        return pc.load_corpus(corpus_path)
    
    def tokenise_corpus(self, corpus_text):
        return pc.tokenize_corpus(corpus_text)

    def word_occurrence(self, corpus_list : list):
        """
        Counts word occurrence in a given corpus, presented as a tokenised word list.
        Returns a dictionary with keys as the tokens and values as the occurrences.
        """
        word_dict = {}
        for word in corpus_list:
            if word in word_dict:
                word_dict[word] += 1
            else:
                word_dict[word] = 1
        return word_dict

    def define_basis_words(self, word_dict : dict, max_words : int):
        """
        Chooses the max_words number of most common words from word_dict
        and return as list for use as basis.
        """
        k = list(word_dict.keys())
        v = list(word_dict.values())
        res_list = []

        for i in range(max_words):
            max_val = max(v)
            val_idx = v.index(max_val)
            res_list.append((k[val_idx],max_val))
            k.remove(k[val_idx])
            v.remove(max_val)

        return res_list

    def map_to_basis(self, corpus_list : list, basis : list, basis_dist_cutoff=10, distance_func=None):
        """
        Maps the words from the corpus into the chosen basis.         
        Returns word_map dictionary, mapping corpus tokens -> basis states

        Keyword arguments:
        corpus_list         --  List of tokens representing corpus
        basis               --  List of basis tokens
        basis_dist_cutoff   --  Cut-off for token distance from basis for it to be significant
        distance_func       --  Function accepting distance between basis and token, and
                                returning the resulting scaling. If 'None', defaults to 
                                1/coeff for scaling param

        """
        if distance_func == None:
            distance_func = lambda x : 1.0/x

        d_map = {}

        # Find location of basis words in token list. Store in dict as d_map
        for b in basis:
            indices = list(filter(lambda x: corpus_list[x] == b[0], range(len(corpus_list))))
            d_map.update({b[0] : np.array(indices)})

        word_map = {}
        # map distance between basis words and other words in token list
        for idx, word in enumerate(corpus_list):
            for b_k, b_v in d_map.items():
                # Basis elements are orthogonal
                if(b_k == word):
                    word_map.update({b_k : {b_k : [0]}})
                    break
                dist = np.abs(b_v - idx)
                m = (word, b_k, [i for i in dist if i <= basis_dist_cutoff])
                if len(m[2]) != 0:
                    if(word_map.get(m[0]) != None):
                        update_val = word_map.get(m[0])
                        update_val.update({m[1] : m[2]})
                        word_map.update({m[0] : update_val })
                    else:
                        word_map.update({m[0] : {m[1] : m[2]} })

            #word_map.get(word)

        return word_map

    def map_to_bitstring(self, basis : list):
        upper_bound_bitstrings = int(np.ceil(np.log2(len(basis))))
        bit_map = {}
        bitstring = 0 # Assume |0...0> state reserved for initialisation only
        for k, v in basis:
            bitstring += 1
            bit_map.update({k: bitstring})
        return (upper_bound_bitstrings, bit_map)

if __name__ == "__main__":
    CorpusFile = os.sys.argv[1]

    dcc = DisCoCat()

    # Load the corpus
    corpus_text = dcc.load_corpus(CorpusFile)
    tokens = dcc.tokenise_corpus(corpus_text)

    basis_v = dcc.define_basis_words(tokens['verbs'], 10)
    basis_n = dcc.define_basis_words(tokens['nouns'], 10)

    bit_map_v = dcc.map_to_bitstring(basis_v)
    bit_map_n = dcc.map_to_bitstring(basis_n)

    print(bit_map_v)
    print(bit_map_n)

#    word_oc_v = dcc.word_occurrence(tokens['tk_words'])

    verb_map = dcc.map_to_basis(tokens['tk_words'], basis_v)
    noun_map = dcc.map_to_basis(tokens['tk_words'], basis_n)

    string_format_bit = '|{:0%db}>'%(bit_map_v[0])
    for k,v in verb_map.items():
        print (k,"-> ", end='')
        coeffs = []
        mapped = []
        for kk,vv in v.items():
            coeffs.append(np.sum([1.0/(ii+1) for ii in vv]))
            mapped.append(string_format_bit.format(bit_map_v[1][kk]))
        norm_coeff = np.linalg.norm(coeffs)
        for i in range(len(mapped)):
            print(coeffs[i]/norm_coeff, mapped[i], " ", end="")
        print("")

#