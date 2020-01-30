# `encoding` - encodes a set of unique bit patterns into an equal superposition of states

The `encoding` directory contains a C++ application which encodes a set of unique bit patterns represented by unsigned integers, and encodes them into a superposition of states. Each state has an equal amplitude.

Optional Arguments (order is relevant):

1. Verbosity - [Bool] 1 on, 0 off (default 0)
2. Number of experiments (shots) - [Unsigned Integer] (default 1000)
3. Length of bit strings to encode - [Unsigned Integer] (default 4)

Application can be launched on a HPC system using SLURM by using and making appropriate adjustments to the SLRUM run script `intel-qnlp/demos/run_scripts/run_script_exe.sh`, and then running the adjusted script.
