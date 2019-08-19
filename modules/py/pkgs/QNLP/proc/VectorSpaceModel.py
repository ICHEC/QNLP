import QNLP.proc.process_corpus as pc
from os import path

class VSM_pc:
    def __init__(self):
        self.pc = pc
    
    def tokenize_corpus(self, corpus, proc_mode=0, stop_words=True):
        """
        Rewrite of pc.tokenize_corpus to allow for tracking of basis word 
        positions in list to improve later pairwise distance calculations.
        """

        token_sents = self.pc.nltk.sent_tokenize(corpus) #Split on sentences
        token_words = [] # Individual words
        tags = [] # Words and respective tags
        
        for s in token_sents:
            tk = self.pc.nltk.word_tokenize(s)
            if stop_words == False:
                tk = self.pc.remove_stopwords(tk, self.pc.sw)
            token_words.extend(tk)
            tags.extend(self.pc.nltk.pos_tag(tk))

        if proc_mode != 0:
            if proc_mode == 's':
                s = self.pc.nltk.SnowballStemmer('english', ignore_stopwords = not stop_words)
                token_words = [s.stem(t) for t in token_words]
            elif proc_mode == 'l':
                wnl = self.pc.nltk.WordNetLemmatizer()
                token_words = [wnl.lemmatize(t) for t in token_words]

        tagged_tokens = self.pc.nltk.pos_tag(token_words)
        ## Consider creating method for the following lines
        d = {}
        # This awful line tracks the positions where a tagged element is found in the tokenised corpus list. Useful for comparing distances
        # If the key doesn't initially exist, it adds a list with a single element. Otherwise, extends the list with the new token position value.
        # d.update( { i[0]: d.get(i[0]).extend(pos) if d.get(i[0]) != None else [pos] for pos,i in enumerate(tagged_tokens) if pc.tg.matchables(pc.tg.Noun, i[1])})

        nouns = self._get_token_position(tagged_tokens, self.pc.tg.Noun)
        verbs = self._get_token_position(tagged_tokens, self.pc.tg.Verb)

        # nouns = [i[0] for i in tagged_tokens if self.pc.tg.matchables(self.pc.tg.Noun, i[1])]
        # verbs = [i[0] for i in tagged_tokens if self.pc.tg.matchables(self.pc.tg.Verb, i[1])]

        count_nouns = { k:len(v) for k,v in nouns.items()}
        count_verbs = { k:len(v) for k,v in verbs.items()}
        ##

        return {'verbs':count_verbs, 'nouns':count_nouns, 'tk_sentence':token_sents, 'tk_words':token_words}

    def _get_token_position(self, tagged_tokens, token_type):
        token_dict = {}
        for pos, token in enumerate(tagged_tokens):
            if pc.tg.matchables(token_type, token[1]):
                if token_dict.get(token[0]) != None:
                    token_dict.get(token[0]).append(pos)
                else:
                    token_dict.update( { token[0] : [pos]} )
        return token_dict


class VectorSpaceModel:
    """
    Use vector space model of meaning to determine relative order of tokens in basis (see disco papers).
    Plan:

    -   1. Populate set of tokens of type t in corpus; label as T; O(n)
    -   2. Choose n top most occurring tokens from T, and define as basis elements of type t; 2 <= n <= |T|; O(n)
    -   3. Find relative (pairwise) distances between these tokens, and define as metric; n(n-1)/2 -> O(n^2)
    -   4. Sort tokens by distance metric; O(n*log(n))
    -   5. Assign tokens integer ID using Gray code mapping of sorted index; O(n)

    After the aboves steps the elements are readily available for encoding using the respective ID. Tokens that appear
    relatively close together will be closer in the sorted list, and as a result have a smaller number of bit flips of difference
    which can be used in the Hamming distance calculation later for similarity of meanings.
    """
    def __init__(self, corpus_path="", mode=0, stop_words=True):
        self.pc = VSM_pc()
        self.tokens = self.load_tokens(corpus_path, mode, stop_words)

    def load_tokens(self, corpus_path, mode=0, stop_words=True):
        " 1. Wraps the calls from process_corpus.py to tokenize. Returns None if path is "
        if path.exists(corpus_path):
            return self.pc.tokenize_corpus( pc.load_corpus(corpus_path), mode, stop_words)
        else:
            return None
    
    def sort_basis_helper(self, token_type, num_elems):
        basis_dict = {token_type : {} }
        #consider Counter.most_common(num_elems)
        for counter, elem in enumerate(sorted(self.tokens[token_type].items(), key=lambda x : x[1], reverse = True)):
            if counter == num_elems:
                break
            basis_dict[token_type].update({elem[0] : elem[1]})
        return basis_dict

    def define_basis(self, num_basis = {"verbs": 8, "nouns":8}):
        """ 2. Specify the number of basis elements in each space. 
        Dict holds keys of type and values of number of elements to request."""

        if self.tokens == None:
            return
        basis = {}
        for t in ("nouns", "verbs"):
            basis.update( self.sort_basis_helper(t, num_basis[t]) )
        return basis

    def calc_token_dist(self, tokens, dist_metric):
        " 3. & 4."
        pass

    def assign_indexing(self, tokens):
        " 5. "
        pass
