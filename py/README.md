`tagging.py` offers a simplified module for checking tagged data types. It condenses many of the types offered by NLTK into a simpler type-set.

For a simple test case, run:
```bash
python ./tag_file.py <corpus.txt> <s/l>
```
where `l` lemmatizes the tokens, and `s` stems the tokens. If no `s/l` option argument given, text is tokenised and tagged without processing. 
