# Building QNLP

Before building, the repository must be cloned to it's final location. We depend on conda internally for the Python environment and associated libraries, so the directory will not be moveable following the setup procedure.


```bash
git clone https://github.com/ichec/qnlp
cd qnlp
```

# Easy-mode: Docker (all platforms)

If you have Docker available on your system, the QNLP suite can be built atop a Ubuntu image using gcc. While we do not expect this to give the best performance, it can be useful as a means to test the functionality and play around.

```bash
docker build .
```

That's it! To get access to the environment, use:

```bash
# In host
docker run -p 8888:8888 -it <image_name> bash # Ports on host XXXX:XXXX on image

# In image
. /qnlp/load_env.sh
jupyter notebook password
jupyter notebook --ip 0.0.0.0 --no-browser --allow-root # server runs as root in image, but not host
```

Access the notebook server on `localhost:8888' as per usual. Note, if you have a jupyter notebook server running at 8888 the above will fail. Change the ports to an unused set and try again.

### Note:
As we do not support Windows directly, this is the preferred method to run on Windows platforms. If you are running WSL (Windows Subsystem for Linux) we expect that the Linux installation should work, but make no promises.

# Intermediate-mode: Linux
This is the preferred mode for installation, as we can better optimise the binaries through use of the Intel Compiler suite. If unavailable, we can also use gcc and/or clang. As a prerequisite for installation, we must have available the following toolkits and packages:

- Compiler toolkit with C++14 support, though C++17 preferentially (tested with icpc >= 2019u3, g++ >= 7.0, clang++ >= 9.0)
- MPI library and compiler wrappers (mpiicpc, mpicxx). We have tested successfully with Intel MPI, MPICH, and OpenMPI.
- CMake 3.15+ available [here](https://cmake.org/download/).
- Internet access on installation device. As some HPC nodes have no external access, it is recommended to install on login nodes to overcome this. 

### Note: the next step might take a while to execute.

Before we can build the suite, we must acquire some dependencies. The file `setup_env.sh` performs this purpose, acquiring all the necessary packages (including Intel-QS), setting up the conda Python environment, and the runtime environment variables. All external dependencies will be installed in `intel-qnlp/third_party/` and `intel-qnlp/third_party/install/`. Upon installation, a `load_env.sh` file is created, which when sourced sets all associated paths and variables to build and run QNLP scripts and binaries.


As there can be many different cases of needs and uses for the QNLP library, we have added many features that can be built in by enabling CMake build flags. The set of available parameters are

- `-DENABLE_TESTS=1`: Builds the suite of unit and integration tests for the library using [catch2](https://github.com/catchorg/Catch2).
- `-DENABLE_LOGGING=1`: Builds support for gate call logging. I/O slows down computation, but the output can be used in conjunction with the Python `circuit_printer.py` script to output a latex-compilable circuit (uses `quantikz` package).
- `-DENABLE_PYTHON=1`: Enable build of the QNLP Python bindings using `pybind11`. These will be installed into the associated conda environment `intel-qnlp`, and available upon sourcing `load_env.sh`.
- `-DENABLE_MPI=1`: Enable the MPI build. While MPI compiler wrappers are *always* required to build the library, by setting this to `0` we can use an OpenMP variant of the backend library, with the threads controlled by `OMP_NUM_THREADS=<set number of threads here>`. MPI should be enabled for distributed usage, or on systems with large processor counts. For smaller systems (laptops and desktops), OpenMP is preferred.
- `-DIqsMPI=1`: This should be enabled if `-DENABLE_MPI=1`, and disabled otherwise. It sets the mode of operation of the underlying Intel-QS simulator. Currently this option expects that you are using the Intel MPI Compiler (`mpiicpc`).
- `-DIqsMKL=1`: If using the Intel Compiler, this enables MKL support for operations.
- `-DENABLE_NATIVE=1`: This allows the underlying compiler to generate instructions targeting the architecture of the system being compiled on. For the best performance this should be enabled. Systems supporting AVX2, and AVX512 can see significant performance benefits.
- `-DENABLE_RESOURCE_EST=1`: This turns off all computation calls in the simulator, and tracks the gate calls only. This is useful to obtain a resource estimation for the depth of circuits.
- `-DENABLE_INTEL_LLVM=1`: If using a new variant of the Intel Compiler (2019u5+) we can enable the newly supported LLVM compiler backend by setting this variable.

To run the compilation process on a standard laptop/desktop we recommend the following steps:

```bash
./setup_env.sh
source ./load_env.sh
cd build 
cmake .. \
-DCMAKE_C_COMPILER=mpicc \
-DCMAKE_CXX_COMPILER=mpicxx \
-DENABLE_TESTS=1 \
-DENABLE_LOGGING=0 \
-DENABLE_PYTHON=1 \
-DENABLE_MPI=0 \
-DIqsMPI=OFF \
-DIqsMKL=OFF \
-DENABLE_NATIVE=on \
-DENABLE_RESOURCE_EST=0 \
-DENABLE_INTEL_LLVM=0
make -j4
```

For compilation on a HPC system, with access to Intel MPI and the Intel Compiler (`mpiicpc`), we recommend:

```bash
./setup_env.sh
source ./load_env.sh
cd build 
cmake .. \
-DCMAKE_C_COMPILER=mpiicc \
-DCMAKE_CXX_COMPILER=mpiicpc \
-DENABLE_TESTS=1 \
-DENABLE_LOGGING=0 \
-DENABLE_PYTHON=1 \
-DENABLE_MPI=1 \
-DENABLE_NATIVE=ON \
-DIqsMPI=ON \
-DIqsMKL=ON \
-DENABLE_INTEL_LLVM=0 \
-DENABLE_RESOURCE_EST=0
make -j16
```

# Hard-mode: MacOS
Given the recent move in MacOS (10.15.x) to remove certain header file directories, not all dependencies will compile. As such, it is recommended to install a compiler suite and MPI library using an alternative package management solution (we recommend MacPorts, but we assume brew should also work). One may attempt to build the compilers from source, but this has become incredibly arduous. Assuming an installed MacPorts environment, we had success using the following:

```bash
sudo port install openmpi-devel-gcc9
sudo port install openmpi-devel-gcc9-fortran
sudo port select --set mpi openmpi-devel-gcc9-fortran
```

With the environment correctly setup, the Linux laptop/desktop solution should work fine from this point.

---

# Testing installation

To assess whether the built libraries and binaries are correctly working, we can run some sample tests.

```bash
source ./load_env.sh
OMP_NUM_THREADS=1 ./build/modules/test/tests "[diffusion]"
OMP_NUM_THREADS=1 ./build/modules/test/tests "[ncu]"
```

The above commands should verify the C++ modules work correctly. To verify if the Python modules are working correctly, we can attempt to import the newly built packages into the Python environment, and create a simulator object with 

```bash
source ./load_env.sh
python -c "import QNLP as q; import PyQNLPSimulator as p; num_qubits = 8; sim = p(num_qubits, False); p.PrintStates(\"Test\", []);"
```

The above command will load the necessary modules into the Python environment, create a simulator of 8 qubits, and print out the state coefficients. If all succeeds, the environment has been correctly set. Additionally, we can use start the `jupyter notebook` environment and run a sample notebook, located at `<QNLP_ROOT>/modules/py/nb` to further investigate.