import QNLP.tagging.word_types as t
import nltk
import sys
import numpy as np

from collections import Counter
from nltk.corpus import stopwords
sw = stopwords.words('english')

def remove_stopwords(text, sw):
    return [w for w in text if w not in sw]

'''
Pass the corpus as a string, which is subsequently broken into tokenised sentences.
'''
def tokenize_corpus(corpus, proc_mode=0):
    token_sents = nltk.sent_tokenize(corpus_text) #Split on sentences
    token_words = [] # Individual words
    tags = [] # Words and respective tags

    for s in token_sents:
        tk = nltk.word_tokenize(s)
        tk = remove_stopwords(tk, stopwords.words('english'))
        token_words.extend(tk)
        tags.extend(nltk.pos_tag(tk))

    if proc_mode != 0:
        if proc_mode == 's':
            s = nltk.SnowballStemmer('english', ignore_stopwords=True)
            token_words = [s.stem(t) for t in token_words]
        elif proc_mode == 'l':
            wnl = nltk.WordNetLemmatizer()
            token_words = [wnl.lemmatize(t) for t in token_words]

    tags = nltk.pos_tag(token_words)

    nouns = [i[0] for i in tags if t.matchables(t.Noun, i[1])]
    verbs = [i[0] for i in tags if t.matchables(t.Verb, i[1])]

    count_nouns = Counter(nouns)
    count_verbs = Counter(verbs)
    return {'verbs':count_verbs, 'nouns':count_nouns, 'tk_sent':token_sents, 'tk_word':token_words}


if __name__ == "__main__":
    corpus_text=""
    
    with open(sys.argv[1], 'r') as corpusFile:
        corpus_text=corpusFile.read()
    
    words = tokenize_corpus(corpus_text, proc_mode=0)
  
    nvn_space_size = len(words['nouns'])**2 * len(words['verbs'])
    print ("Nouns count: ", words['nouns'])
    print ("Verbs count: ", words['verbs'])

    print ("S Vec meaning space size: ", nvn_space_size)
    print("Required qubits: ", int(np.ceil(np.log2(nvn_space_size)) ))

    common_n = words['nouns'].most_common()

    for s in words['tk_sent']:
        tokens = nltk.word_tokenize(s)
        tokens = remove_stopwords(tokens, stopwords.words('english'))
        tags = nltk.pos_tag(tokens)

        nouns = [i[0] for i in tags if t.matchables(t.Noun, i[1])]
        verbs = [i[0] for i in tags if t.matchables(t.Verb, i[1])]








    for i,v_i in enumerate(common_n[:len(count_nouns)-1]):
        for j,v_j in enumerate(common_n[i+1:len(count_nouns)]):

            idx_i = np.array([pos for pos,val in enumerate(tokens) if val == v_i[0]])
            idx_j = np.array([pos for pos,val in enumerate(tokens) if val == v_j[0]])
            val=np.Inf


            for ii in idx_i:
                for jj in idx_j:
                    tmp=int(np.abs(jj-ii))
                    #print(tmp)
                    if val>tmp:
                        val = tmp
            #from IPython import embed; embed()
            if (i != j) and (v_i != v_j) and (val < 2) :
                print (nouns[i], nouns[j], i, j, val)
    print(common_n)
    print(tokens)
