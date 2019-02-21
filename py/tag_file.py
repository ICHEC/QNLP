import tagging as t
import nltk
import sys
import numpy as np

if __name__ == "__main__":
    corpus_text=""
    
    with open(sys.argv[1], 'r') as corpusFile:
        corpus_text=corpusFile.read()

    tokens = nltk.word_tokenize(corpus_text)
    tags = nltk.pos_tag(tokens)
    nouns = set([i[0] for i in tags if t.matchables(t.Noun, i[1])])
    verbs = set([i[0] for i in tags if t.matchables(t.Verb, i[1])])
    nvn_space_size = len(nouns)**2 * len(verbs)

    print ("Nouns:", nouns)
    print ("Verbs:", verbs)
    #from IPython import embed; embed()
    print ("S Vec meaning space size:", nvn_space_size)
    print ("Total meaning space size:", len(nouns)**2 * nvn_space_size)
    print("Required qubits:", int(np.ceil(np.log2(nvn_space_size*len(nouns)**2))) )
