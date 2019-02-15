# 25/01/2019
Upcoming plans as discussed in standup:
 - Compute grammatical correctness of a sentence given a corpus.
 - Use sample corpus and define the mappings (vectors space, tensors products, etc) for a given example.
 - Map the graphical structure elements for a given example to Dirac notation.

# 28/01/2019
 - Discussed the implementation of category theory approaches; considering Haskell implementation for prototyping. Also considering Julia.
 - Choose a given small corpus using Python library NLTK
 - BSD/Apache/MIT licenses allowed. GPL only if non shipping solution.

# 30/01/2019
 - Discussed the use of Julia/Haskell as potential for implementation Cat. Theory models. Additionally, embedding/FFI work with C++.
 - Use of Intel advisor for vectorisation info from small-scale run of Intel-QS qft test

# 15/02/2019
 - Discussed the three approaches studied for the closest vector problem
   1. MS QNN (https://arxiv.org/pdf/1401.2142v2.pdf)
   2. Entanglement-based ML on a QC (https://arxiv.org/pdf/1409.7770v3.pdf)
   3. Comparison of 2 QNN on IBM Qiskit (https://file.scirp.org/pdf/NS_2018030915062326.pdf)
 - Agreed to document pros and cons of each (requirement of oracles, solving linear equations to encode problem-specific information into coefficients, CSWAP vs. CNOT approach and its suitability for binary or real number vectors)
 - Agreed to replicate implementation of first approach in [3] for preliminary evaluation (MD)
 - Agreed to separate encode information into coefficients from the rest of NLP implementation; pre-compute coefficients for a representative corpus
 - Selected a simple representative corpus ("Jack and Jill")
   - Tag words with types (LOR)
   - Estimate number of qubits and Q states required for the meaning space for this corpus (LOR, VK)
 - Investigate mapping of sentence similarity algorithm (VK, LOR)