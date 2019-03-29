from nltk.corpus import wordnet as wn
import process_corpus as pc

import qnlp_db

import itertools
from heapq import heappush, heappop

db = qnlp_db.qnlp_db("qnlp_tagged_corpus",".")

basis_nouns = db.db_load("noun", "basis")
basis_verbs = db.db_load("verb", "basis")

basis_nouns_rev = db.db_load("noun", "basis", "reverse")
basis_verbs_rev = db.db_load("verb", "basis", "reverse")

corpus_nouns = db.db_load("noun", "corpus")
corpus_verbs = db.db_load("verb", "corpus")

#As names aren't currently recognised, read in a list of known names, if matched -> noun::person
names = []

#All names return type person
with open("../corpus/names.dat", 'r') as namesFile:
    names=namesFile.read().splitlines()

#Skip the header of the file
names = names[17:]
db.close_db()

#print("NOUNS:\n", basis_nouns)
print("###############################################################################")
#print("VERBS:\n", basis_verbs)

basis_set = set()

class data_tmp:
    def __init__(self,word_list):
        self.word_list = word_list
    def words(self):
        return self.word_list

# Match corpus nouns to basis nouns
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
        basis_set.add(basis_nouns[ci][0])
        # if nothing found, delve deeper for words in the basis
    if len(basis_set) == 0:
        #from IPython import embed; embed()
        sim = []
        a = wn.synsets( ci, pos='n' )
        for bn in basis_nouns.keys():
            b = wn.synsets( bn, pos='n' )
            for s1,s2 in itertools.product(a,b):
                sim.append((bn, wn.wup_similarity(s1,s2)))
        sim.sort(reverse=True, key=lambda x:x[1])
        print(ci, sim[0:5])

    corpus_nouns[ci].append(list(basis_set))
    basis_set.clear()

# Match corpus verbs to basis verbs
for ci in corpus_verbs.keys():
    if (ci.casefold() not in basis_verbs.keys()):
        s_c = wn.synsets(ci)
        for s in s_c:
            spl = s.name().rsplit(".")
            if spl[1] == "v":
                for bn in basis_verbs.keys():
                    if spl[0].casefold() == bn.casefold():
                        basis_set.add(basis_verbs[bn][0])
    else:
        basis_set.add(basis_verbs[ci][0])
    # if nothing found, delve deeper for words in the basis
    if len(basis_set) == 0:
        sim = 0.0
        bs = ""
        a = wn.synsets( ci, pos='v' )
        for bn in basis_verbs.keys():
            b = wn.synsets( bn, pos='v' )
            for s1,s2 in itertools.product(a,b):
                tmp = wn.wup_similarity(s1,s2)
                if (tmp > sim):
                    bs = bn
                    sim = tmp
        print(ci, bs, sim)
    corpus_verbs[ci].append(list(basis_set))
    basis_set.clear()

# Print the matched data
print (corpus_nouns)
for k,v in corpus_nouns.items():
    if(len(v[1]) > 0):
        for j in v[1]:
            print(k, "->", basis_nouns_rev[j])

print (corpus_verbs)
for k,v in corpus_verbs.items():
    if(len(v[1]) > 0):
        for j in v[1]:
            print(k, "->", basis_verbs_rev[j])
#print (corpus_verbs)