# `hamming_RotY` - computes the relative similarity between a test bit pattern and a set of training bit patters

The `hamming_RotY` directory contains a C++ application which encodes a set of specified unique bit patterns (test patterns) into a superposition of states, encodes a test bit pattern into another quantum register that is tensored with each state, then computes the relative similarity between the test bit pattern and each of the training patterns by calculating the Hamming distance and then applying a Y rotation on an auxiliary qubit for each matching bit. The Y rotations adjust the amplitude of a state. Post-selection is then conducted on the auxiliary qubit, followed by a measurement of the register containing the training patterns. Repeating the experiment for a large number of shots builds a distribution of measured states. The frequency of measured states depicts the relative similarity between the test bit pattern and each of the training bit patterns.

Optional Arguments (order is relevant):

1. Verbosity - [Bool] 1 on, 0 off (default 0)
2. Test Pattern - [Unsigned Integer] (default 3)
3. Number of experiments (shots) - [Unsigned Integer] (default 1000)
4. Length of bit strings to encode - [Unsigned Integer] (default 2)

Application can be launched on a HPC system using SLURM by using and making appropriate adjustments to the SLRUM run script `intel-qnlp/demos/run_scripts/run_script_exe.sh`, and then running the adjusted script.
