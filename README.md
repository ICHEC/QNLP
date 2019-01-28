# Quantum Natural Language Processing

#### Start date: 14-Jan-2019, End date: 13-Mar-2020

[Intel](www.intel.com) and [ICHEC](www.ichec.ie) identified the opportunity to leverage the [Intel Quantum Simulator](https://github.com/intel/Intel-QS) (that can simulate up to ~42 qubits [[4]](Documents/Literature/qHiPSTER_paper.pdf)) to port and implement a quantum version of an existing compositional semantics NLP algorithm to analyse the meaning sentences in a corpus. This project aims to
1.	Leverage and evaluate the computing power that quantum devices can offer to computation intensive NLP algorithms such as in the distributional compositional semantics model.
2.	Develop the ecosystem of proof-of-concept applications ported to the emerging quantum computing domain, particularly using a highly relevant application domain such as NLP.
3.	Pioneer a collaborative innovation environment in Ireland between industry and research organisations to develop expertise to program quantum computers.

## Background

### Natural Language Processing
Natural language processing (NLP) is often used to perform tasks like machine translation, sentiment analysis, relationship extraction, word sense disambiguation and automatic summary generation. Most traditional NLP algorithms for these problems are defined to operate over strings of words, and are commonly referred to as the “bag of words” approach. The challenge, and thus limitation, of this approach is that the algorithms analyse sentences in a corpus based on meanings of the component words and lack information from the grammatical rules and nuances of the language. Consequently, the qualities of results of these traditional algorithms are often unsatisfactory when the complexity of the problem increases. On the other hand, an alternate approach called “compositional semantics” incorporates the grammatical structure of sentences in a language into the analysis algorithms. Compositional semantics algorithms include the information flows between words in a sentence to determine the meaning of the whole sentence. One such model is “distributional compositional semantics” (DisCo), which is based on tensor product composition to give a grammatically informed algorithm that computes the meaning of sentences and phrases. This algorithm has been noted to offer significant improvements to the quality of results, particularly for more complex sentences. However, the main challenge in its implementation is the need for large classical computational resources.

### Quantum Computing
Quantum computers have the ability to solve complex problems that are beyond the capabilities of classical computers and will enforce the next genuine disruption to technical computing. The impacts of Quantum Computing will be significantly greater than those brought by many-core architectures and accelerators such as GPUs. While many enterprises (including Intel, IBM, Google, etc.) have been developing physical quantum computing devices, another line of developing quantum computing platforms is through the creation of simulators that are deployed on classical HPC (High Performance Computing) systems. It is widely acknowledged that quantum devices and simulators of size ~50 qubits allow for implementation of proof-of-concept algorithms and have computation power that exceeds many of currently available Peta-scale supercomputers. With the availability of such quantum computing platforms, it is essential that we develop the software ecosystem and programming expertise to target the quantum platforms.

## Project Objective

The distributional compositional semantics (DisCo) model was originally developed by its authors with direct inspiration from quantum theory. The authors of DisCo have developed a quantum version of the algorithm that can be implemented on quantum computers [[1]](Documents/Literature/disco01_paper.pdf), which is based on the DisCo model that was introduced in [[2]](Documents/Literature/disco02_paper.pdf) and [[3]](Documents/Literature/disco03_paper.pdf).

The quantum version of the DisCo model presents two algorithms:
1.	A quantum algorithm for the “closest vector problem”
An algorithm for the “closest vector problem” is used to determine the word/phrase out of a set of words/phrases that has the closest relation (for instance, meaning) to a given word/phrase. This finds application in many computational linguistic tasks such as text classification, word/phrase similarity, test classification and sentiment analysis. A quantum version of this algorithm in [[1]](Documents/Literature/disco01_paper.pdf) enables its implementation on a quantum computing platform.
2.	A quantum algorithm for the “CSC sentence similarity” model
This algorithm is an adaptation of the “closest vector problem” quantum algorithm to perform sentence similarity calculations in the distributional compositional framework. This algorithm is based on tensor product composition that gives a grammatically informed algorithm to compute meaning of sentences/phrases and stores the meanings in quantum systems. Based on this, a quantum implementation of this algorithm has lower storage and compute requirements compared to a classic HPC implementation [[1]](Documents/Literature/disco01_paper.pdf).

In this project, we will implement the two quantum algorithms (“closest vector problem” and “CSC sentence similarity”) of the DisCo model on the Intel Quantum Simulator (qHiPSTER). Given a corpus, the implemented solution will be able to compute the meanings of two sentences (built from words in the corpus) and decide if their meanings match.

In this project, we estimate to target corpuses with ~2000 most common words using the Intel Quantum Simulator (qHiPSTER) that can simulate ~42 qubits. qHiPSTER will be installed on the Irish national supercomputer (Kay).

## Project Execution

### Task 1 - Resource Setup and Problem Mapping
- **Objective**: Install and test the Intel Quantum Simulator (qHiPSTER) on the ICHEC supercomputer (Kay), and define the strategy to map the quantum version of the DisCo model algorithm on the Intel Quantum Simulator.
  - T1.1. Install and test the Intel Quantum Simulator (qHiPSTER) on the ICHEC supercomputer (Kay).
  - T1.2. Investigate the quantum version of the distributed compositional semantics (DisCo) model algorithms, and define their mapping strategy on qHiPSTER.
  - T1.3. Specify the methodology to test and evaluate the implementation.
  - T1.4. Prepare a representative corpus (10x – 100x words) for preliminary testing and evaluation.
- **Deliverables**
  - D1.1 (M03): A report describing the abstraction of the quantum DisCo model algorithms for implementation on any quantum platform, and their mapping on qHiPSTER, along with testing and evaluation methodologies, and the representative corpora.
- **Milestones**
  - M1.1 (M01): qHiPSTER is installed and tested on ICHEC’s Kay.
  - M1.2 (M03): Abstraction and mapping of DisCo algorithms on qHiPSTER is complete.
  - M1.3 (M03): Testing and evaluation methodologies are defined.
  - M1.4 (M03): Representative corpora for testing are ready.
  
### Task 2 - Solution Development
- **Objective**: Implement the DisCo module algorithms on qHiPSTER and perform preliminary evaluations.
  - T2.1. Implement and test the quantum algorithm for the “closest vector problem” in the DisCo model on qHiPSTER based on the mapping strategy defined in T1.2.
  - T2.2. Evaluate the “closest vector problem” implementation using the representative corpora based on the evaluation methodology specified in T1.3.
  - T2.3. Implement and test the quantum algorithm for the “CSC sentence similarity” in the DisCo model on qHiPSTER based on the mapping strategy defined in T1.2.
  - T2.4. Evaluate the “CSC sentence similarity” implementation using the representative corpora based on the evaluation methodology specified in T1.3.
- **Deliverables**
  - D2.1 (M07): A report summarising the implementation, testing and evaluation of the “closest vector problem” algorithm using the representative corpora.
  - D2.2 (M11): A technical presentation at an Intel event at SC 2019.
  - D2.3 (M11): A report summarising the implementation, testing and evaluation of the “CSC sentence similarity” algorithm using the representative corpora.
- **Milestones**
  - M2.1 (M07): Implementation of the abstract and qHiPSTER-specific code, and evaluation for the “closest vector problem” is complete.
  - M2.2 (M11): Implementation of the abstract and qHiPSTER-specific code, and evaluation for the “CSC sentence similarity” algorithm is complete.

### Task 3 - Solution Evaluation and Packaging
- **Objective**: Perform extended evaluations and package the software solution along with results and documentation.
  - T3.1. Prepare a larger corpus (1000x words) for final testing and evaluation. Evaluate the final implementation of the DisCo model algorithms using the larger corpus.
  - T3.2. Package the final version of the implementation with documentation and evaluation results.
- **Deliverables**
  - D3.1 (M14): A report describing the larger corpora, and summarising the final evaluations using the larger corpora.
  - D3.2 (M14): Final version of the abstract interface code for the DisCo algorithms, with documentation.
  - D3.3 (M14): Final version of the qHiPSTER-specific code for the DisCo algorithms, with documentation.
  - D3.4 (M14): A technical poster and presentation for ISC 2020.
- **Milestones**
  - M3.1 (M14): Larger corpora for final testing and evaluation is ready.
  - M3.2 (M14): Final version of the DisCo algorithms implementations and evaluation is complete.

### Task 4 - Technical Report and Handover
- **Objective**: Delivery of the software solutions and results to Intel.
  - The final documented version of the DisCo algorithm implemented on qHiPSTER.
  - All deliverables prepared in Tasks 1, 2 and 3.
- **Deliverables**
  - D4.1 (M14): A consolidation of reports from deliverables D1.1, D2.1, D2.2, D2.3, D3.1, D3.3 and final software from deliverables D3.2 and D3.3.
- **Milestones**
  - M4.1 (M14): Consolidated reports and final software are prepared.
  - M4.2 (M14): Deliverable D4.1 is published on a GitHub page under the Apache License version 2.0.

## Project Timeline

| Task / Month | M01 (14-Jan) | M02 (14-Feb) | M03 (14-Mar) | M04 (14-Apr) | M05 (14-May) | M06 (14-Jun) | M07 (14-Jul) | M08 (14-Aug) | M09 (14-Sep) | M10 (14-Oct) | M11 (14-Nov) | M12 (14-Dec) | M13 (14-Jan) | M14 (14-Feb) |
| - | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| T1.1 | `Y` |  |  |  |  |  |  |  |  |  |  |  |  |  |
| T1.2 | `Y` | `Y` | `Y` |  |  |  |  |  |  |  |  |  |  |  |
| T1.3 |  |  | `Y` |  |  |  |  |  |  |  |  |  |  |  |
| T1.4 |  |  | `Y` |  |  |  |  |  |  |  |  |  |  |  |
| T2.1 |  |  |  | `Y` | `Y` | `Y` | `Y` |  |  |  |  |  |  |  |
| T2.2 |  |  |  |  |  | `Y` | `Y` |  |  |  |  |  |  |  |
| T2.3 |  |  |  |  |  |  |  | `Y` | `Y` | `Y` | `Y` |  |  |  |
| T2.4 |  |  |  |  |  |  |  |  |  | `Y` | `Y` |  |  |  |
| T3.1 |  |  |  |  |  |  |  |  |  |  |  | `Y` | `Y` | `Y` |
| T3.2 |  |  |  |  |  |  |  |  |  |  |  |  |  | `Y` |
| T4   |  |  |  |  |  |  |  |  |  |  |  |  |  | `Y` |

## References

[1]. William Zeng and Bob Coecke, “[Quantum Algorithms for Compositional Natural Language Processing](Documents/Literature/disco01_paper.pdf)”, Proceedings of SLPCS, 2016.

[2]. Stephen Clark, Bob Coecke and Mehrnoosh Sadrzadeh, “[A Compositional Distributional Model of Meaning](Documents/Literature/disco02_paper.pdf)”, Proceedings of 2nd Quantum Interaction Symposium, 2008.

[3]. Bob Coecke, Mehrnoosh Sadrzadeh and Stephen Clark, “[Mathematical Foundations of a Compositional Distributional Model of Meaning](Documents/Literature/disco03_paper.pdf)”, Special issue of Linguistic Analysis, 2010.

[4]. [Intel Quantum Simulator (qHiPSTER)](Documents/Literature/qHiPSTER_paper.pdf).