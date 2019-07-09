###############################################################################

import sqlite3
import os
from typing import Dict, Tuple
import process_corpus as pc
import numpy as np
from qnlp_db import qnlp_db

###############################################################################

#Use mixin to modify the insert statements and modify the structure of database
class qdb_mixin(qnlp_db):

    def create_table_discocat(self, table_name="qnlp"):
        """
        Create the database table for tagging the required data. The DB has columns
        for dataset ('basis' or 'corpus'), data_type ('verb', 'noun', etc.), token (the string value), mapping_idx (the index of the mapped binary value; for superpos states, this index labels the number of values in the superposition), map_bin_id (the binary value representing the quantum state in the register), map_coeff_r/i (real and imaginary coefficients of the map_bin_id state), mapping_dir (indicates the direction of the mapping; may not be used).
        """
        cr_tbl = """CREATE TABLE {}(
                    id INTEGER PRIMARY KEY, 
                    dataset TEXT,
                    data_type TEXT,
                    token TEXT,
                    mapping_idx INTEGER,
                    map_bin_id INTEGER,
                    map_coeff_r REAL,
                    map_coeff_i REAL,
                    mapping_dir INTEGER
                    );""".format(table_name)
        conn = super(qdb_mixin, self).connect_db()
        c = conn.cursor()

        try:
            c.execute(cr_tbl)

        except sqlite3.OperationalError as oe:
            remove_db = input("Table '{}' already exists. Remove? y/n: ".format(table_name))
            if remove_db is "y":
                self.drop_table(table_name)
                self.create_table_discocat(table_name)

        except Exception as e:
            print("SQLITE exception thrown: {0}".format(e), "Exiting program.")
            exit()

        finally:
            conn.commit()

    def db_insert_discocat(self, values, dataset="basis", data_type="noun", table_name="qnlp"):
        """
        Insert the tag to binary encoding mapping into the DB.

        values      -- Dict mapping string to binary value, and binary value to string.
        data_type   -- String to indicate the type of data to be stored
        table_name  -- Name of table to store in DB
        """

        '''
        The DB insert operation below assumes the value field of a key in DB is a tuple,
        containing (binary_id, weight of occurrence), where weight of occurrence cant be
        determined by the proximity of the word to other words; essentially a count in the 
        simplest case. The mapping checks to see if the index is convertible to a numeric
        type. If so, this will imply the reverse mapping (ie qubit result to string val), 
        and is indicated by -1. Otherwise, this will be a forward mapping, and given by 1.
        '''
        conn = super(qdb_mixin, self).connect_db()
        c = conn.cursor()
        self.create_table_discocat(table_name)
        
        for corpus_token, superpos in values.items():
            l_superpos = len(superpos)
            for idx, (distance_measure, basis_state) in enumerate(superpos):
                c.execute("""INSERT INTO {} ( 
                    dataset, 
                    data_type, 
                    token, 
                    mapping_idx, 
                    map_bin_id, 
                    map_coeff_r, 
                    map_coeff_i, 
                    mapping_dir ) VALUES(?,?,?,?,?,?,?,?)""".format(table_name), 
                    (dataset, 
                    data_type, 
                    corpus_token, 
                    idx, 
                    basis_state, 
                    distance_measure.real, 
                    distance_measure.imag, 
                    0)
                )
        conn.commit()

###############################################################################

class DisCoCat:
    """
    Implements precomputation for the DisCo(Cat) model to represent sentence meanings
    using category theory methods. See <PAPERS> for details.
    """
    def __init__(self, fd = lambda x : [1.0/(i+1) for i in x]):
        self.distance_func = fd

    def load_corpus(self, corpus_path):
        return pc.load_corpus(corpus_path)
    
    def tokenise_corpus(self, corpus_text):
        return pc.tokenize_corpus(corpus_text)

###############################################################################

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

###############################################################################

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

###############################################################################

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
            distance_func = lambda x : [1.0/i for i in x]

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
        return word_map

###############################################################################

    def map_to_bitstring(self, basis : list):
        upper_bound_bitstrings = int(np.ceil(np.log2(len(basis))))
        bit_map = {}
        bitstring = 0 # Assume |0...0> state reserved for initialisation only
        for k, v in basis:
            bitstring += 1
            bit_map.update({k: bitstring})
        return (upper_bound_bitstrings, bit_map)

###############################################################################

    def generate_state_mapping(self, bit_map, dat_map):
        """
        Takes the basis bitstring map, and the token-to-basis relationship, and returns a normalised set of states, with coefficients determined by the distance_func lambda, given the distance between the token and the resulting basis element.
        """
        num_states = bit_map[0]

        # Mapping token to array of tuples, first index the basis state coefficient and second the integer representation of the bitstring state
        state_encoding = {}
        for token, basis_dist_map in dat_map.items():
            local_coeffs = []
            local_states = []
            for basis_token, distance_list in basis_dist_map.items():
                # If more than one occurrence for the same word, apply the distance relation function then sum the results for that basis work coefficient
                local_coeffs.append( np.sum( self.distance_func(distance_list) ) )
                local_states.append( bit_map[1][basis_token] )

            # Calc normalisation factor over all the respective basis states for a given token
            norm_factor = np.linalg.norm(local_coeffs)
            for state_idx in range( len(local_states) ):
                # Normalise the coefficient
                local_coeffs[state_idx] /= norm_factor
                current = state_encoding.get(token)
                if current != None:
                    current.append( (local_coeffs[state_idx], local_states[state_idx],) )
                else:
                    state_encoding.update({token : [(local_coeffs[state_idx], local_states[state_idx],)] })
        return state_encoding

    def latex_states(self, bit_map, dat_map, file_name = "state"):
        """
        LaTeX file outputter for state generation. Given the above data structures, file_name.tex is generated. Beware, as output may need to replace '_' with '\_' for non-math-mode usage.
        """

        mapping = self.generate_state_mapping(bit_map, dat_map)
        with open(file_name + ".tex", "w") as f:
            f.write("\\documentclass{article} \n \\usepackage{amsmath} \\usepackage{multicol} \n \\begin{document} \n")
            tex_string_format_bit = r'\vert {:0%db} \rangle'%(bit_map[0])
            f.write("\\section{Basis} \\begin{multicols}{2} \n \\noindent ")
            for b_key, b_val in bit_map[1].items():
                f.write(b_key + " $\\rightarrow " + tex_string_format_bit.format(b_val) + "$\\\\ ")
            f.write("\\end{multicols}")
            f.write("\\noindent\\rule{\\textwidth}{1pt} \n")
            f.write("\\noindent\\rule{\\textwidth}{1pt} \n")
            f.write("\\section{Encoding} \n")
            for token, basis_map in mapping.items():
                f.write(r"\begin{align}\vert \textrm{" + token + "} \\rangle &= \\\\ \n &" )
                for i,b in enumerate(basis_map):
                    if( i != 0 ):
                        if(i%3 == 0):
                            f.write(r" \\ & ")
                    f.write("{0:.3f}".format(round(b[0],3)))
                    f.write(tex_string_format_bit.format(b[1]) )
                    if(i != len(basis_map) - 1):
                        f.write(r"+")
                    f.write(" \\nonumber ")
                f.write(r"""\end{align}""")
                f.write("\\noindent\\rule{\\textwidth}{1pt} \n")
            f.write(r"\end{document}")

###############################################################################

if __name__ == "__main__":
    CorpusFile = os.sys.argv[1]

    dcc = DisCoCat()
    db = qdb_mixin(os.path.basename(CorpusFile),'.')

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

    n_states = dcc.generate_state_mapping(bit_map_n, noun_map)
    v_states = dcc.generate_state_mapping(bit_map_n, noun_map)

    dcc.latex_states(bit_map_n, noun_map, "nouns")
    dcc.latex_states(bit_map_v, verb_map, "verbs")

    db.db_insert_discocat(n_states, "corpus", "noun", "corpus")
    db.db_insert_discocat(v_states, "corpus", "verb", "corpus")

###############################################################################