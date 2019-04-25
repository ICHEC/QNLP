import nltk
import sys
import numpy as np

from collections import Counter
from nltk.corpus import stopwords

from tagging.word_types import Noun
from tagging.word_types import Verb
from tagging.word_types import Sentence

__all__ = ["word_types"]

"""
Reduces the nltk types to simplified types defined above
"""    
def matchables(classType, tag):
    if isinstance(classType, Noun) or classType is Noun:
        return tag in ["NN","NNS","NNP","NNPS"]
    elif isinstance(classType, Verb) or classType is Verb:
        return tag in ["VB", "VBD", "VBG", "VBN", "VBP", "VBZ"]
    else:
        return False        
