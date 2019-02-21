from tagging import *
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
