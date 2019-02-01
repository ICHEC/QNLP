### Note: this is a work in progress

# QNLP software stack design notes

In no particular order, we may require the following (albeit somewhat currently vague) functionality from the QNLP software:

1. String parsing and tokenization of a text corpus.
2. Text preprocessing and tagging to correct types following pregroup grammars.
3. <del>Grammar analysis of adjacent string types for correctness.</del>
4. Resulting vector space inference and problem space definition for quantum implementation.
5. Quantum state generation of the resulting space, and resulting simulation.
6. Interpretation of results from quantum simulation.

Given a top-down approach, each element above can be considered coupled only to its nearest neighbour. As such, we may consider different 
technologies to implement the layers, provided they are compatible. <del>In essence, we are currently considering Haskell and Julia for the
category theory layers (2,3,4)</del>. Additionally, given the powerful tools already existing for NLP, the string parsing and tokenization work
(1) may benefit from Python, though modules also exist for Haskell and Julia. The quantum layers (5,6) will most certainly be in C++,
though with appropriate bindings we can call the respective functions/methods from upper layers (Julia, Haskell, Python are all well equipped for this).
