from nltk.corpus import wordnet as wn
import QNLP.proc.process_corpus as pc

import QNLP.io.qnlp_db as qnlp_db

import itertools
import sys

import os


if __name__ == "__main__":
    db = None

    if len(sys.argv) > 2:
        db_file = os.sys.argv[2]
        db = qnlp_db.qnlp_db( os.path.basename(db_file), os.path.dirname(db_file) )
    else:
        db = qnlp_db.qnlp_db("qnlp_tagged_corpus", ".")

    basis_nouns = db.db_load("noun", "basis")
    basis_verbs = db.db_load("verb", "basis")

    basis_nouns_rev = db.db_load("noun", "basis", "reverse")
    basis_verbs_rev = db.db_load("verb", "basis", "reverse")

    corpus_nouns = db.db_load("noun", "corpus")
    corpus_verbs = db.db_load("verb", "corpus")

    #As names aren't currently recognised, read in a list of known names, if matched -> noun::person
    #names = []

    #All names return type person
    #with open("../corpus/names.dat", 'r') as namesFile:
    #    names=namesFile.read().splitlines()

    #Skip the header of the file
    #names = names[17:]
    db.close_db()

    #print("NOUNS:\n", basis_nouns)
    print("###############################################################################")
    #print("VERBS:\n", basis_verbs)

    basis_set = set()

    def match_syn(word, basis_dat, pos_type=None, deep_search=False):
        """Calculates the synonym set of a given word, and attempts to match the meanings."""
        basis_set = set()
        syn = wn.synsets(word, pos=pos_type)
        for s in syn:
            spl = s.name().rsplit(".")
            for bn in basis_dat.keys():
                if spl[0].casefold() == bn.casefold():
                    basis_set.add(basis_dat[bn][0])

        #If nothing found, perform next closest match using similarity measures between all basis terms and current synonyms. Very expensive!
        if len( basis_set ) == 0 and deep_search == True and len(syn) > 0:
            sim=[]
            for b in basis_dat.keys():
                b_syn = wn.synsets(b, pos=pos_type)
                for s1,s2 in itertools.product(syn, b_syn):
                    sim.append((b, wn.wup_similarity(s1,s2)))
            sim.sort(reverse=True, key=lambda x:x[1])
            basis_set.add(basis_dat[sim[0][0]][0])
        return basis_set

    if len(sys.argv) > 1 and sys.argv[1] == "t":
        deep_search = True
    else:
        deep_search = False

    # Match corpus nouns to basis nouns
    for ci in corpus_nouns.keys():
        #Match directly, or first level synonyms
        #if ci in names:
            #basis_set.add( basis_nouns["person"][0] )
        if 0:
            pass
        elif (ci.casefold() in [b.casefold() for b in basis_nouns.keys()]):
            basis_set.add( basis_nouns[ci][0]) 
        else:
            basis_set.update( match_syn(ci, basis_nouns, pos_type='n', deep_search=deep_search) )
        corpus_nouns[ci].append(list(basis_set))
        basis_set.clear()

    # Match corpus verbs to basis verbs
    for ci in corpus_verbs.keys():
        if (ci.casefold() in [b.casefold() for b in basis_verbs.keys()]):
            basis_set.add(basis_verbs[ci][0])
        else:
            basis_set.update( match_syn(ci, basis_verbs, pos_type='v', deep_search=deep_search) )
        corpus_verbs[ci].append(list(basis_set))
        basis_set.clear()

    # Print the matched data
    from tabulate import tabulate
    print("###############################################################################")
    print("############################## Noun encodings #################################")
    print("###############################################################################")
    d = [(k,v[0],[bin(i) for i in v[1]]) for k,v in corpus_nouns.items()]
    print(tabulate(d,["Token","ID","Encoding"]))
    print("")
    #print (corpus_nouns)
    for k,v in corpus_nouns.items():
        if(len(v[1]) > 0):
            for j in v[1]:
                print(k, "->", basis_nouns_rev[j])
    print("")
    print("###############################################################################")
    print("############################## Verb encodings #################################")
    print("###############################################################################")
    d = [(k,v[0],[bin(i) for i in v[1]]) for k,v in corpus_verbs.items()]
    print(tabulate(d,["Token","ID","Encoding"]))
    print("")
    #print (corpus_verbs)
    for k,v in corpus_verbs.items():
        if(len(v[1]) > 0):
            for j in v[1]:
                print(k, "->", basis_verbs_rev[j])
    print("")
