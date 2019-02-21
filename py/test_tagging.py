from tagging import *
import nltk

n1 = Noun(False)
n2 = Noun(True,1)
v1 = Verb(False)
s1 = Sentence([n1,v1,n2])

v2 = Verb(True,-2)

print(v2)
print(n1)
print(n2)
print(v1)
print(s1)

jnj="""
Jack and Jill went up the hill
To fetch a pail of water.
Jack fell down and broke his crown,
And Jill came tumbling after.
"""
tokens = nltk.word_tokenize(jnj)

print("################################")
print("Matching nouns")
print("################################")
for ii in nltk.pos_tag(tokens):
    print(ii[0],":",matchables(n1, ii[1]))

print()
print("################################")
print("Matching verbs")
print("################################")
for ii in nltk.pos_tag(tokens):
    print(ii[0],":",matchables(v1, ii[1]))
