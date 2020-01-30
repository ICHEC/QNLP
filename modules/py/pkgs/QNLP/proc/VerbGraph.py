
import networkx as nx
import numpy as np

class VerbGraph:
    """
    Used to define subject/object noun ordering from a given verb.
    Creates a left/right dict of nouns, wherein given a threshold,
    verbs with respective nouns within that threshold on either side 
    can be thought of as subject (left) or object (right) for 
    bitstring state generation for encoding.
    """
    def __init__(self, verb = None, pos = []):
        self.verb = verb
        self.pos = pos
        self.left_nouns = {}
        self.right_nouns = {}
        self.lr_nouns = {}
        
    def addL(self, noun, pos):
        self.left_nouns.update({noun : pos})
        
    def addR(self, noun, pos):
        self.right_nouns.update({noun : pos})
        
    def createLGraph(self, G = None):
        v_idx = []
        l_idx = []
        if G == None:
            G = nx.DiGraph()
        G.add_node(self.verb)
        v_idx = self.verb
        for k,v in self.left_nouns.items():
            G.add_node(k)
            l_idx.append(k)
            G.add_edge(k, self.verb)
        return G, v_idx, l_idx
    
    def createRGraph(self, G = None):
        v_idx = []
        r_idx = []
        if G == None:
            G = nx.DiGraph()
        G.add_node(self.verb)
        v_idx = self.verb

        for k,v in self.right_nouns.items():
            G.add_node(k)
            r_idx.append(k)
            G.add_edge(self.verb, k)
        return G, v_idx, r_idx
    
    @staticmethod
    def calc_verb_noun_pairings(corpus_list_v, corpus_list_n, dist_cutoff, max_terms=5):
        v_list = []
        for word_v, locations_v in corpus_list_v.items():

            if len(locations_v[1]) <= 1:
                continue

            v = VerbGraph(word_v)
            lv = locations_v[1][:, np.newaxis]
            for word_n, locations_n in corpus_list_n.items():#######!!!!!!!#######
                dists = np.ndarray.flatten(locations_n[1] - lv)
                if not isinstance(dists, np.int64):
                    vals = [x for x in dists if np.abs(x) <= dist_cutoff]
                else:
                    vals = [dists] if np.abs(dists) <= dist_cutoff else []
                
                dl = [(i,i_pos) for i,i_pos in zip(vals, locations_n[1]) if i < 0]
                dr = [(i,i_pos) for i,i_pos in zip(vals, locations_n[1]) if i > 0]
                
                if len(dl) >0:
                    v.addL(word_n, dl)
                if len(dr) >0:
                    v.addR(word_n, dr)
                    
            lr_dist = 0
            for kl,vl in v.left_nouns.items(): 
                for kr,vr in v.right_nouns.items():
                    lr_dist = vr[0][1] - vl[0][1]
                    if lr_dist < 2*dist_cutoff and lr_dist > 0: #>0 assumes correct left-right ordering
                        v.lr_nouns.update({(kl,kr) : lr_dist})
            if len(v.left_nouns) >1 and len(v.right_nouns) >1 and len(v.lr_nouns) >1:
                v_list.append(v)
        return v_list