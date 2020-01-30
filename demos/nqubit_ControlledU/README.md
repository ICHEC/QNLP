# `nqubit_ControledU` - applies an n-qubit controlled unitary operation to a qubit

The `nqubit_ControlledU` directory contains a C++ application which computes an n-qubit controlled unitary gate call on another qubit. This is an example of a higher order gate operation. The nCU gate call is decomposed into a series of one and two qubit gate calls. Note: the number of one and two qubit gate calls required after the decomposition grows polynomially as the number of control qubits increase.

Apply NCU with input data as control on a target qubit in state 0. The U matrix will be PailiX for simplicity. Outputs the final measured state. Displays proportion of experiments with target qubit set.

Arguments:
    - None

Application can be launched on a HPC system using SLURM by using and making appropriate adjustments to the SLRUM run script `intel-qnlp/demos/run_scripts/run_script_exe.sh`, and then running the adjusted script.
