`tagging.py` offers a simplified module for checking tagged data types. It condenses many of the types offered by NLTK into a simpler type-set.

For a simple test case, run:
```bash
python ./tag_file.py <corpus.txt> <s/l>
```
where `l` lemmatizes the tokens, and `s` stems the tokens. If no `s/l` option argument given, text is tokenised and tagged without processing. 


# Constructing basis, and mapping words to that basis
As an example of defining a basis set of words, we opt for Ogden's "Basic English", about 850 words which can (according to the author)
be used to construct all other word meanings. Using NLTK and WordNet, we can create unique binary strings for this basis set, and map
a given corpus to use these basis terms to contruct their meaning space.

If a given term in the corpus has more than one matching term in the basis, then the resulting corpus term will require a superposition of these states to fully describe it.

We can process a given corpus against a given basis as follows:
```bash
python ./process_corpus.py ../corpus/Ogden856.dat ../corpus/jack_and_jill.txt l
```
where `Ogden856.dat` is the basis, and `jack_and_jill.txt` is the corpus. The above command populates a database with the tagged terms from both.
```bash
python ./basis_check.py f
```
subsequently loads the data from the database, runs a comparison between both data sets, and outputs the indices of the basis (mapped to binary for quantum bit-string representation) that matches the terms best.

A given flag can be set at runtime `t/f` which specifies to perform a deep search if the required matching term is not found. This can be rather slow for large datasets, so setting to false is recommended by default.
