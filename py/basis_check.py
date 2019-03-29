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
    if len( basis_set ) == 0 and deep_search == True:
        sim=[]
        for b in basis_dat.keys():
            b_syn = wn.synsets(b, pos=pos_type)
            for s1,s2 in itertools.product(syn, b_syn):
                sim.append((b, wn.wup_similarity(s1,s2)))
        sim.sort(reverse=True, key=lambda x:x[1])
        print(word, sim[0:5])
        x = iter(sim)
        #Take top three elements if they exist
        basis_set.add(next(x)[0])
        basis_set.add(next(x)[0])
        basis_set.add(next(x)[0])
    return basis_set

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
