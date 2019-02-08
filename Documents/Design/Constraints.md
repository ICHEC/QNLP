# IN PROGRESS NOTES

## qRAM and oracle designs
It is worth noting that an actual circuit-based implementation of qRAM may be unrealistic given the limited number of qubits available. The discussion and subsequent implementation of a ["bucket-brigade" quantum RAM model](https://arxiv.org/pdf/0708.1879.pdf) requires a number of features:
- Hierarchical scheme using qutrit states to access the required memory location.
- Oracle-based routine to choose the correct state to access.

Following the work of https://arxiv.org/pdf/1502.03450.pdf, wherein they propose a realisable qubit-based circuit model for a small number of states, it can be seen that the required qubits grows quickly, as well as requiring potentially unrealistic qubit access routines. 

Additionally, automatic generation of oracles to enable the required access patterns can potentially [be difficult](https://arxiv.org/pdf/quant-ph/9503016.pdf). Assuming a model requiring $`n`$-qubit controlled NOT gates, we require an additional $`n-2`$ qubits to use as anciliary states during the evaluation.

As such, it may be best to assume the oracles as functions that perform the required mappings, given a set of inputs, as opposed to an actual circuit-based model, given the few qubits available to us for the problem space. Additionally, keeping track of the qubit registers in a classical addressing scheme, and choosing the appropriate address to extract a value(s) may be required.