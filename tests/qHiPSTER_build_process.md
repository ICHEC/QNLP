# Steps to build https://github.com/intel/Intel-QS on Kay

```bash
git clone https://github.com/intel/Intel-QS.git
cd Intel-QS
export QHIPSTER_ROOT=$PWD
```

Next, load the Intel Parallel Studio environment and build the qHipSTER library.
```bash
# Loads Intel Parallel Studio XE 2018 update 4 as the default on Kay
module load intel
make all
```

Using the provided tests, we modified and submitted `sbatchscript.edison.36qubits.sh` to the queue. The job fails to work. The log/error files show the resulting output, and are found on Kay at `/ichec/work/ichec001/output36qubit/`.

We next run the same test, but with fewer qubits (28) and nodes (8), which runs to success. We suspect the issue is the MPI message size, and next attempt to resolve this using a BigMPI-enabled build.

Initially we set the flag in `make.inc` for `bigmpi = 1` to enable a build of the BigMPI software. However, as this only enables support for BigMPI were it already built, running `make all` at this stage returns errors as it looks for `bigmpi.hpp`. Next, the assumption of the BigMPI lib and include directories at `/opt/bigmpi` are manually set within this same file. Ideally, it would be best if BigMPI were built automatically as part of the qHiPSTER build process, and installed into the `build` directory as if running `make sdk-release`. This would remove the need for manually defined paths, and allow us to create a more automated build process.

To build BigMPI we next run the following commands from the `${QHIPSTER_ROOT}` directory:
```bash
mkdir ${QHIPSTER_ROOT}/build
cd ${QHIPSTER_ROOT}/util/BigMPI/
```

For a static library of `libbigmpi.a` we can use the autotools build steps, which are slightly modified from the ones given by the [BigMPI documentation](https://github.com/intel/Intel-QS/blob/consistent-naming/util/BigMPI/INSTALL.md). We install the BigMPI library into the Intel-QS build directory.
```bash
./autogen.sh;
./configure CC=mpiicc --prefix=${QHIPSTER_ROOT}/build;
make && make install;
```

For a dynamic library of `libbigmpi.so` we can use the CMake build process and perform the installation into the same directory as previously (we can also build a static library using CMake, but the default is shared).
