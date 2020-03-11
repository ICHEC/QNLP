# Jupyter notebooks

The current notebooks directory assumes a fully installed Python distribution with the QNLP packages. Please ensure you run `source load_env.sh` before launching the jupytyer notebook server.

## Contents:

- **QNLP_Python.ipynb**: The quintessential introduction to the implemented QNLP methods. This notebook demontrates the strategy for representing sentence structures in quantum states, and demonstrates the encoding of a test-pattern within the learned meaning space.
- **QuantumGateEncoding.ipynb**: As a demontration of the interoperability between the Python-wrapped C++ methods and Numpy, we show how to perform operations using the QNLP gate models. Additionally, some simple utilities are also presented here.
- **EntanglementVectorCompare_py.ipynb**: This notebook was an early investigation into encoding data directly into state coefficients. While many of the methods are now obsolete, for the simple test-cases given the notebook attempts to follow the work of PRL 114 110504 (2015), and implements methods to represent floating point values in the state coefficfients. 
- **PostprocParams.ipynb**: This notebook gives a quick discussion on the preprocessor parameter choice, and performs some simple analysis on the outcomes. 
- **nCU_decomposition.ipynb**: A quick demonstration of the decomposition of higher-order controlled gates to 2-qubit gate calls. 
- **TensorOps_jl.ipynb**: Some early Julia-based notebook for tensoring of meaning spaces.
