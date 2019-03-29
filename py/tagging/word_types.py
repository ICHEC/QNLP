###############################################################################
__author__ = "Lee J. O'Riordan (ICHEC)"
__copyright__ = 'Copyright 2019, Q-NLP'
__credits__ = ['{credit_list}']
__license__ = '{license}'
__version__ = '{mayor}.{minor}.{rel}'
__maintainer__ = '{maintainer}'
__email__ = 'lee.oriordan@ichec.ie'
__status__ = 'alpha'
###############################################################################

"""
Base class for pregroup types
"""
class PregroupType():
    def __init__(self, label):
        self.label = label
    def __str__(self):
        return self.label

###############################################################################

"""
Simple class for pregroup types with adjoint
""" 
class AdjointType(PregroupType):
    def __init__(self, label, isAdj, adjOrder=0):
        super().__init__(label)
        self.isAdj = isAdj
        if isAdj:
            assert (adjOrder!=0)
        self.adjOrder = adjOrder
        
    def __str__(self):
        adj_order_str = ""
        if self.isAdj:
            adj_order_str = "^"

            for ii in range(abs(self.adjOrder)):
                adj_order_str += 'l' if self.adjOrder < 0 else 'r'
                
        return super().__str__() + ": ADJ=" +  str(self.isAdj) + " ORD=" +  adj_order_str  

###############################################################################

"""
Meaning data type
"""
class Meaning(AdjointType):
    def __init__(self, isAdj, adjOrder=0):
        super().__init__('s', isAdj, adjOrder)

###############################################################################

"""
Noun data type with matched nltk type defined in matchables()
"""
class Noun(AdjointType):
    def __init__(self, isAdj, adjOrder=0):
        super().__init__('n', isAdj, adjOrder)

###############################################################################

"""
Verb data type (n^r \otimes s \otimes n^l) with matched nltk type defined in matchables()
"""
class Verb(AdjointType):
    def __init__(self, isAdj, adjOrder=0,   left_adj : Noun = None, 
                                            right_adj : Noun = None, 
                                            meaning : Meaning = None):
        super().__init__('v', isAdj, adjOrder)
        self.left_adj = left_adj
        self.right_adj = right_adj
        self.meaning = meaning

###############################################################################

"""
Simple sentence data type
"""
class Sentence():
    def __init__(self, structure):
        self.structure = structure
    def __str__(self):
        return 's:' + ','.join([s.label for s in self.structure])

###############################################################################