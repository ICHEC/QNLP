import os
from QNLP.proc.DisCoCat import *

if __name__ == "__main__":
    CorpusFile = os.sys.argv[1]
    basis_length = int(os.sys.argv[2])

    dcc = DisCoCat(fd = lambda x : [1.0 for i in x])
    db = qdb_mixin(os.path.basename(CorpusFile),'.')

    # Load the corpus
    corpus_text = dcc.load_corpus(CorpusFile)
    tokens = dcc.tokenise_corpus(corpus_text)

    basis_v = dcc.define_basis_words(tokens['verbs'], basis_length)
    basis_n = dcc.define_basis_words(tokens['nouns'], basis_length)

    bit_map_v = dcc.map_to_bitstring(basis_v)
    bit_map_n = dcc.map_to_bitstring(basis_n)

    print(bit_map_v)
    print(bit_map_n)

#    word_oc_v = dcc.word_occurrence(tokens['tk_words'])

    verb_map = dcc.map_to_basis(tokens['tk_words'], basis_v, 10, distance_func = lambda x : [1.0 for i in x])
    noun_map = dcc.map_to_basis(tokens['tk_words'], basis_n, 10, distance_func = lambda x : [1.0 for i in x])

    n_states = dcc.generate_state_mapping(bit_map_n, noun_map)
    v_states = dcc.generate_state_mapping(bit_map_n, noun_map)

    dcc.latex_states(bit_map_n, noun_map, "nouns")
    dcc.latex_states(bit_map_v, verb_map, "verbs")

    db.db_insert_discocat(n_states, "corpus", "noun", "corpus")
    db.db_insert_discocat(v_states, "corpus", "verb", "corpus")

###############################################################################