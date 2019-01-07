# Intel Quantum Natural Language Processing (Intel QNLP)

#### Start date: 14-Jan-2019, End date: 13-Mar-2020

## Overview

Intel and ICHEC identified the opportunity to leverage the Intel Quantum Simulator (that can simulate up to ~42 qubits) to port and implement a quantum version of an existing compositional semantics NLP algorithm to analyse the meaning sentences in a corpus. This project aims to
1.	Leverage and evaluate the computing power that quantum devices can offer to computation intensive NLP algorithms such as in the distributional compositional semantics model.
2.	Develop the ecosystem of proof-of-concept applications ported to the emerging quantum computing domain, particularly using a highly relevant application domain such as NLP.
3.	Pioneer a collaborative innovation environment in Ireland between industry and research organisations to develop expertise to program quantum computers.

The distributional compositional semantics (DisCo) model explained above was originally developed by its authors with direct inspiration from quantum theory. Following this, the authors of DisCo have since co-developed a quantum version of the algorithm that can be implemented on quantum computers [1], which is based on the DisCo model that was introduced in [2] and [3].

The quantum version of the DisCo model presents two algorithms:
1.	A quantum algorithm for the “closest vector problem”
An algorithm for the “closest vector problem” is used to determine the word/phrase out of a set of words/phrases that has the closest relation (for instance, meaning) to a given word/phrase. This finds application in many computational linguistic tasks such as text classification, word/phrase similarity, test classification and sentiment analysis. A quantum version of this algorithm in [1] enables its implementation on a quantum computing platform.
2.	A quantum algorithm for the “CSC sentence similarity” model
This algorithm is an adaptation of the “closest vector problem” quantum algorithm to perform sentence similarity calculations in the distributional compositional framework. This algorithm is based on tensor product composition that gives a grammatically informed algorithm to compute meaning of sentences/phrases and stores the meanings in quantum systems. Based on this, a quantum implementation of this algorithm has lower storage and compute requirements compared to a classic HPC implementation [1].

In this project, we will implement the two quantum algorithms (“closest vector problem” and “CSC sentence similarity”) of the DisCo model on the Intel Quantum Simulator (qHiPSTER). Given a corpus, the implemented solution will be able to compute the meanings of two sentences (built from words in the corpus) and decide if their meanings match.

In this project, we estimate to target corpuses with ~2000 most common words using the Intel Quantum Simulator (qHiPSTER) that can simulate ~42 qubits. qHiPSTER will be installed on the Irish national supercomputer (Kay).

## Tasks

### Task 1

- Task Title: Resource Setup and Problem Mapping
- Task Owner: ICHEC 100%
- Task Team Members / % of time on task: 1 Research Fellow (20%), 2 Post-docs (each 100%)
- Task Objective: Install and test the Intel Quantum Simulator (qHiPSTER) on the ICHEC supercomputer (Kay), and define the strategy to map the quantum version of the DisCo model algorithm on the Intel Quantum Simulator.
- Describe in detail how the task will be completed: 
This work package will be implemented through the following four tasks:
  - T1.1. Install and test the Intel Quantum Simulator (qHiPSTER) on the ICHEC supercomputer (Kay).
  - T1.2. Investigate the quantum version of the distributed compositional semantics (DisCo) model algorithms, and define their mapping strategy on qHiPSTER.
  - T1.3. Specify the methodology to test and evaluate the implementation.
  - T1.4. Prepare a representative corpus (10x – 100x words) for preliminary testing and evaluation.
- Duration: 12 weeks
- **Deliverables**
  - D1.1 (M03): A report describing the abstraction of the quantum DisCo model algorithms for implementation on any quantum platform, and their mapping on qHiPSTER, along with testing and evaluation methodologies, and the representative corpora.
- **Milestones**
  - M1.1 (M01): qHiPSTER is installed and tested on ICHEC’s Kay.
  - M1.2 (M03): Abstraction and mapping of DisCo algorithms on qHiPSTER is complete.
  - M1.3 (M03): Testing and evaluation methodologies are defined.
  - M1.4 (M04): Representative corpora for testing are ready.
  
### Task 2

- Task Title: Solution Development
- Task Owner: ICHEC 100%
- Task Team Members / % of time on task: 1 Research Fellow (20%), 2 Post-docs (each 100%)
- Task Objective: Implement the DisCo module algorithms on qHiPSTER and perform preliminary evaluations.
- Describe in detail how the task will be completed: 
This work package will be implemented through the following four tasks:
  - T2.1. Implement and test the quantum algorithm for the “closest vector problem” in the DisCo model on qHiPSTER based on the mapping strategy defined in T1.2.
  - T2.2. Evaluate the “closest vector problem” implementation using the representative corpora based on the evaluation methodology specified in T1.3.
  - T2.3. Implement and test the quantum algorithm for the “CSC sentence similarity” in the DisCo model on qHiPSTER based on the mapping strategy defined in T1.2.
  - T2.4. Evaluate the “CSC sentence similarity” implementation using the representative corpora based on the evaluation methodology specified in T1.3.
- Duration: 32 weeks
- **Deliverables**
  - D2.1 (M07): A report summarising the implementation, testing and evaluation of the “closest vector problem” algorithm using the representative corpora.
  - D2.2 (M11): A technical presentation at an Intel event at SC 2019.
  - D2.3 (M11): A report summarising the implementation, testing and evaluation of the “CSC sentence similarity” algorithm using the representative corpora.
- **Milestones**
  - M2.1 (M07): Implementation of the abstract and qHiPSTER-specific code, and evaluation for the “closest vector problem” is complete.
  - M2.2 (M11): Implementation of the abstract and qHiPSTER-specific code, and evaluation for the “CSC sentence similarity” algorithm is complete.

### Task 3

- Task Title: Solution Evaluation and Packaging
- Task Owner: ICHEC 100%
- Task Team Members / % of time on task: 1 Research Fellow (20%), 2 Post-docs (each 100%)
- Task Objective: Perform extended evaluations and package the software solution along with results and documentation.
- Describe in detail how the task will be completed: 
This work package will be implemented through the following two tasks:
  - T3.1. Prepare a larger corpus (1000x words) for final testing and evaluation. Evaluate the final implementation of the DisCo model algorithms using the larger corpus.
  - T3.2. Package the final version of the implementation with documentation and evaluation results.
- Duration: 10 weeks
- **Deliverables**
  - D3.1 (M14): A report describing the larger corpora, and summarising the final evaluations using the larger corpora.
  - D3.2 (M14): Final version of the abstract interface code for the DisCo algorithms, with documentation.
  - D3.3 (M14): Final version of the qHiPSTER-specific code for the DisCo algorithms, with documentation.
  - D3.4 (M14): A technical poster and presentation for ISC 2020.
- **Milestones**
  - M3.1 (M14): Larger corpora for final testing and evaluation is ready.
  - M3.2 (M14): Final version of the DisCo algorithms implementations and evaluation is complete.

### Task 4
- Task Title: Technical Report and Handover
- Task Owner: ICHEC 100%
- Task Team Members / % of time on task: 1 Research Fellow (20%), 2 Post-docs (each 100%)
- Task Objective: Delivery of the software solutions and results to Intel.
- Describe in detail how the task will be completed: 
In this work package, ICHEC will deliver the following components to Intel:
  - The final documented version of the DisCo algorithm implemented on qHiPSTER.
  - All deliverables prepared in Tasks 1, 2 and 3.
- Duration: 2 weeks
- **Deliverables**
  - D4.1 (M14): A consolidation of reports from deliverables D1.1, D2.1, D2.2, D2.3, D3.1, D3.3 and final software from deliverables D3.2 and D3.3.
- **Milestones**
  - M4.1 (M14): Consolidated reports and final software are prepared.
  - M4.2 (M14): Deliverable D4.1 is published on a GitHub page under the Apache License version 2.0.


## Timeline

| Task | M01 (14-Jan) | M02 (14-Feb) | M03 (14-Mar) | M04 (14-Feb) | M05 (14-Feb) | M06 (14-Feb) | M07 (14-Feb) | M08 (14-Feb) | M09 (14-Feb) | M10 (14-Feb) | M11 (14-Feb) | M12 (14-Feb) | M13 (14-Feb) | M14 (14-Feb) |
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

[1]. William Zeng and Bob Coecke, “Quantum Algorithms for Compositional Natural Language Processing”, Proceedings of SLPCS, 2016.

[2]. Stephen Clark, Bob Coecke and Mehrnoosh Sadrzadeh, “A Compositional Distributional Model of Meaning”, Proceedings of 2nd Quantum Interaction Symposium, 2008.

[3]. Bob Coecke, Mehrnoosh Sadrzadeh and Stephen Clark, “Mathematical Foundations of a Compositional Distributional Model of Meaning”, Special issue of Linguistic Analysis, 2010.
