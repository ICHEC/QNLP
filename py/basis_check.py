from nltk.corpus import wordnet as wn
import process_corpus as pc

import qnlp_db

db = qnlp_db.qnlp_db("qnlp_tagged_corpus",".")

basis_nouns = db.db_load("noun", "basis")
basis_verbs = db.db_load("verb", "basis")

corpus_nouns = db.db_load("noun", "corpus")
corpus_verbs = db.db_load("verb", "corpus")
names = []

#All names return type person
with open("/Users/mlxd/workspace/quantum/NLP/Data/Names/combined_names_sorted.txt", 'r') as namesFile:
    names=namesFile.read().splitlines()

db.close_db()

print("NOUNS:\n", basis_nouns)
print("###############################################################################")
print("VERBS:\n", basis_verbs)

if(0):
    for ci in corpus_nouns.keys():
        #corpus_nouns[ci] = []
        if ci in names:
            corpus_nouns[ci].append(basis_nouns["person"][0])
            #print(ci, "person", basis_nouns["person"])
            #from IPython import embed; embed()
            continue
        if (ci.casefold() not in basis_nouns.keys()):
            s_c = wn.synsets(ci)
            for s in s_c:
                spl = s.name().rsplit(".")
                if spl[1] == "n":
                    for bn in basis_nouns.keys():
                        if spl[0].casefold() == bn.casefold():
                            corpus_nouns[ci].append(basis_nouns[bn][0])

                        #print(spl[0], bn)

            #[syn.casefold() for syn in s.lemma_names() if syn.casefold() in basis_nouns.keys()]:
                #print(s,ci)
                #break
#from IPython import embed; embed()

#for ci in corpus_verbs.keys():
#    if (ci not in basis_verbs.keys()):
#        s_c = wn.synsets(ci)
#        for s in s_c:
#            if s.pos() == "v" and [syn.casefold() for syn in s.lemma_names() if syn.casefold() in basis_verbs.keys()]:
#                print(s,ci)
#                break

basis_set = set()
for ci in corpus_nouns.keys():
    if ci in names:
        basis_set.add(basis_nouns["person"][0])
    elif (ci.casefold() not in basis_nouns.keys()):
        s_c = wn.synsets(ci)
        for s in s_c:
            spl = s.name().rsplit(".")
            if spl[1] == "n":
                for bn in basis_nouns.keys():
                    if spl[0].casefold() == bn.casefold():
                        basis_set.add(basis_nouns[bn][0])
    else:
        basis_set.add(basis_nouns[bn][0])
    corpus_nouns[ci].append(list(basis_set))
    basis_set.clear()


for ci in corpus_verbs.keys():
    basis_set = set()
    if (ci.casefold() not in basis_verbs.keys()):
        s_c = wn.synsets(ci)
        for s in s_c:
            spl = s.name().rsplit(".")
            if spl[1] == "v":
                for bn in basis_verbs.keys():
                    if spl[0].casefold() == bn.casefold():
                        basis_set.add(basis_verbs[bn][0])
    else:
        basis_set.add(basis_verbs[bn][0])
    corpus_verbs[ci].append(list(basis_set))

print (corpus_nouns)
print (corpus_verbs)