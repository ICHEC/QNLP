# Convert corpus to non-zero state coefficients for quantum encoding

import QNLP.tagging as tg
import os

import nltk
import sys
import numpy as np

# DB for exporting data to be read into C++
import QNLP.io.qnlp_db as qdb

from collections import Counter
from nltk.corpus import stopwords
sw = stopwords.words('english')

###################################

def remove_stopwords(text, sw):
    """Remove words that do not add to the meaning; simplifies sentences"""
    return [w for w in text if w not in sw]

###################################

def tokenize_corpus(corpus, proc_mode=0, stop_words=True):
    """
    Pass the corpus as a string, which is subsequently broken into tokenized sentences, and returned as dictionary of verbs, nouns, tokenized words, and tokenized sentences.

    Keyword arguments:
    corpus      -- string representing the corpus to tokenize
    proc_mode   -- defines the processing mode. Lemmatization: proc_mode=\"l\";  Stemming: proc_mode=\"s\"; No additional processing: proc_mode=0 (default=0)
    stop_words  -- indicates whether stop words should be removed (False) or kept (True) (from nltk.corpus.stopwords.words(\"english\"))
    """

    token_sents = nltk.sent_tokenize(corpus) #Split on sentences
    token_words = [] # Individual words
    tags = [] # Words and respective tags

    for s in token_sents:
        tk = nltk.word_tokenize(s)
        if stop_words == False:
            tk = remove_stopwords(tk, sw)
        token_words.extend(tk)
        tags.extend(nltk.pos_tag(tk))

    if proc_mode != 0:
        if proc_mode == 's':
            s = nltk.SnowballStemmer('english', ignore_stopwords = not stop_words)
            token_words = [s.stem(t) for t in token_words]
        elif proc_mode == 'l':
            wnl = nltk.WordNetLemmatizer()
            token_words = [wnl.lemmatize(t) for t in token_words]

    tags = nltk.pos_tag(token_words)
    nouns = [i[0] for i in tags if tg.matchables(tg.Noun, i[1])]
    verbs = [i[0] for i in tags if tg.matchables(tg.Verb, i[1])]

    count_nouns = Counter(nouns)
    count_verbs = Counter(verbs)

    return {'verbs':count_verbs, 'nouns':count_nouns, 'tk_sentence':token_sents, 'tk_words':token_words}

###################################

def load_corpus(corpus_path, proc_mode=0):
    """Load the corpus from disk."""
    corpus_text=""
    with open(corpus_path, 'r') as corpusFile:
        corpus_text=corpusFile.read()
    return corpus_text

def process(corpus_path, proc_mode=0):
    """Tokenize the corpus data."""
    words = tokenize_corpus(load_corpus(corpus_path), proc_mode=proc_mode)
    return words

###################################

def word_pairing(words, window_size):
    """
    Examine the words around each verb with the specified window size, and attempt to match the NVN pattern. The window_size specifies the number of values around each verb to search for the matching nouns. If passed as tuple
    (l,r) gives the left and right windows separately. If passed as a scalar, both values are equal.

    Keyword arguments:
    words       -- list of the tokenized words
    window_size -- window to search for word pairings. Tuple
    """

    #Naively set sentence boundary at location of full-stops.
    sentence_boundary = set([i for i, x in enumerate(words['tk_words']) if "." in x])

    if type(window_size) is tuple:
        window_left, window_right = window_size
    else:
        window_left = window_size
        window_right = window_size
    verb_idx={}
    
    for v in words['verbs']:
        verb_idx.update({v:[i for i, x in enumerate(words['tk_words']) if v in x]})
    
    for n in words['nouns']:
        for v,k in verb_idx.items():
            for v_idx in k:
                #Ensure that no full stop appears over the windowed region to avoid crossing sentences.
                l_left = len(sentence_boundary.intersection(range(v_idx-window_left,v_idx)))
                l_right = len(sentence_boundary.intersection(range(v_idx,v_idx+window_right)))
                
                if l_left==0 and n in words['tk_words'][v_idx-window_left:v_idx]:
                    print("LEFT:=",n,v)
                if l_right==0 and (v_idx + window_right) < len(words['tk_words']) and n in words['tk_words'][v_idx:v_idx+window_right] :
                    print("RIGHT:=",v,n)

###################################

def num_qubits(words):
    """
    Calculate the required number of qubits to encode the NVN state-space of the corpus.
    Returns tuple of required states and number of needed qubits. 
    While it is possible to reduce qubit count by considering the total required number as being the product of each combo, it is easier to deal with separate states for nouns and verbs, with at most 1 extra qubit required.
    
    Keyword arguments:
    words       -- list of the tokenized words
    """
    nvn_space_size = len(words['nouns'])**2 * len(words['verbs'])
    req_qubits_n = int(np.ceil(np.log2(len(words['nouns']))))
    req_qubits_v = int(np.ceil(np.log2(len(words['verbs']))))
    
    #req_qubits = int(np.ceil(np.log2(nvn_space_size)))
    req_qubits = req_qubits_n*2 + req_qubits_v
    
    print("Unique states:",nvn_space_size,"\tRequired qubits total:", req_qubits,           "\tRequired qubits nouns:", req_qubits_n, "\tRequired qubits verbs:", req_qubits_v)
    return (nvn_space_size, req_qubits, req_qubits_n, req_qubits_v)

def mapNameToBinaryBasis(words, db_name, table_name="qnlp"):
    """
    Maps the unique string in each respective space to a binary basis number for qubit representation.

    Keyword arguments:
    words       -- list of the tokenized words
    db_name     -- name of the database file
    table_name  -- name of the table to store data in db_name
    """

    _, num_q_total, num_q_n, num_q_v = num_qubits(words)

    verbMap = {}
    nounMap = {}
    for i,v in enumerate(words['nouns']):
        #Perform two-way mapping for bin to value and value to bin
        nounMap.update({v:format(i,'b').zfill(num_q_n)})
        nounMap.update({format(i,'b').zfill(num_q_n):v})

    for i,v in enumerate(words['verbs']):
        verbMap.update({v:format(i,'b').zfill(num_q_v)})
        verbMap.update({format(i,'b').zfill(num_q_v):v})
        
    db = qdb.qnlp_db(db_name, ".")
    db.db_insert(nounMap, "noun", table_name)
    db.db_insert(verbMap, "verb", table_name)
    
    return (nounMap, verbMap)

###################################

if __name__ == "__main__":
    if len(os.sys.argv) < 2:
        raise("Please specify the path to basis words as arg 1, corpus as arg 2, and processing mode as arg 3")

    BasisPath = os.sys.argv[1]
    CorpusPath = os.sys.argv[2]
    
    if len(os.sys.argv) == 4:
        proc_mode = os.sys.argv[3]
    else:
        proc_mode=0

    # Load the basis words
    basis_text=""    
    # Load the corpus
    corpus_text=""

    with open(BasisPath, 'r') as basisFile:
        basis_text=basisFile.read()

    with open(CorpusPath, 'r') as corpusFile:
        corpus_text=corpusFile.read()

    basis_words = tokenize_corpus(basis_text, proc_mode=proc_mode)
    corpus_words = tokenize_corpus(corpus_text, proc_mode=proc_mode)

    #Naively set sentence boundary at location of full-stops.
    sentence_boundary = set([i for i, x in enumerate(corpus_words['tk_words']) if "." in x])

    # examine word windowing for proximity
    #window = (2,3)
    window=1
    word_pairing(basis_words, window)

    # Determine qubit requirements
    _, num_q_total, num_q_n, num_q_v = num_qubits(basis_words)

    #Map basis words to binary strings, and populate DB
    basis_nMap, basis_vMap = mapNameToBinaryBasis(basis_words, "qnlp_tagged_corpus","basis")

    corpus_nMap, corpus_vMap = mapNameToBinaryBasis(corpus_words, "qnlp_tagged_corpus","corpus")
