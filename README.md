# QNLP - Quantum Natural Language Processing

#### Start date: 14-Jan-2019, End date: 13-Mar-2020

[Intel](www.intel.com) and [ICHEC](www.ichec.ie) identified the opportunity to leverage the [Intel Quantum Simulator](https://github.com/intel/Intel-QS) (that can simulate up to ~42 qubits [4,5] to port and implement a quantum version of an existing compositional semantics NLP algorithm to analyse the meaning sentences in a corpus. This project aims to
1.	Leverage and evaluate the computing power that quantum devices can offer to computation intensive NLP algorithms such as in the distributional compositional semantics model.
2.	Develop the ecosystem of proof-of-concept applications ported to the emerging quantum computing domain, particularly using a highly relevant application domain such as NLP.
3.	Pioneer a collaborative innovation environment in Ireland between industry and research organisations to develop expertise to program quantum computers.

## Background

### Natural Language Processing
Natural language processing (NLP) is often used to perform tasks like machine translation, sentiment analysis, relationship extraction, word sense disambiguation and automatic summary generation. Most traditional NLP algorithms for these problems are defined to operate over strings of words, and are commonly referred to as the “bag of words” approach. The challenge, and thus limitation, of this approach is that the algorithms analyse sentences in a corpus based on meanings of the component words and lack information from the grammatical rules and nuances of the language. Consequently, the qualities of results of these traditional algorithms are often unsatisfactory when the complexity of the problem increases. On the other hand, an alternate approach called “compositional semantics” incorporates the grammatical structure of sentences in a language into the analysis algorithms. Compositional semantics algorithms include the information flows between words in a sentence to determine the meaning of the whole sentence. One such model is “distributional compositional semantics” (DisCo), which is based on tensor product composition to give a grammatically informed algorithm that computes the meaning of sentences and phrases. This algorithm has been noted to offer significant improvements to the quality of results, particularly for more complex sentences. However, the main challenge in its implementation is the need for large classical computational resources.

### Quantum Computing
Quantum computers have the ability to solve complex problems that are beyond the capabilities of classical computers and will enforce the next genuine disruption to technical computing. The impacts of Quantum Computing will be significantly greater than those brought by many-core architectures and accelerators such as GPUs. While many enterprises (including Intel, IBM, Google, etc.) have been developing physical quantum computing devices, another line of developing quantum computing platforms is through the creation of simulators that are deployed on classical HPC (High Performance Computing) systems. It is widely acknowledged that quantum devices and simulators of size ~50 qubits allow for implementation of proof-of-concept algorithms and have computation power that exceeds many of currently available Peta-scale supercomputers. With the availability of such quantum computing platforms, it is essential that we develop the software ecosystem and programming expertise to target the quantum platforms.

### QNLP
In this software suite we have developed a workflow to implement hybridised classical-quantum models for representing language and calculating sentence meanings. 

## References

[1]. William Zeng and Bob Coecke, “[Quantum Algorithms for Compositional Natural Language Processing](https://arxiv.org/pdf/1608.01406.pdf)”, Proceedings of SLPCS, 2016.

[2]. Stephen Clark, Bob Coecke and Mehrnoosh Sadrzadeh, “[A Compositional Distributional Model of Meaning](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.363.8703&rep=rep1&type=pdf)”, Proceedings of 2nd Quantum Interaction Symposium, 2008.

[3]. Bob Coecke, Mehrnoosh Sadrzadeh and Stephen Clark, “[Mathematical Foundations of a Compositional Distributional Model of Meaning](http://arxiv.org/pdf/1003.4394v1.pdf)”, Special issue of Linguistic Analysis, 2010.

[4]. [qHiPSTER: The Quantum High Performance Software Testing Environment](https://arxiv.org/abs/1601.07195).

[5]. [Intel Quantum Simulator: A cloud-ready high-performance simulator of quantum circuits](https://arxiv.org/abs/2001.10554).


