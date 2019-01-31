# Steps to build https://github.com/intel/Intel-QS on Kay
Upon logging into Kay, it is safe to assume that we can build the following files from our home directory. Additionally, these can be built within the project shared directory at `/ichec/work/ichec001/<BUILD_DIR>.` We begin by cloning the repository, and exporting an environment variable (`${QHIPSTER_ROOT}`) with a path to the repo.

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

Initially we set the flag in `make.inc` for `bigmpi = 1` to enable a build of the BigMPI software. However, as this only enables support for BigMPI were it already built, running `make all` at this stage returns errors as it looks for `bigmpi.hpp`. Next, the assumption of the BigMPI lib and include directories at `/opt/bigmpi` are manually set within this same file. Ideally, it would be best if BigMPI were built automatically as part of the qHiPSTER build process, and installed into an `install` directory (`build` was considered, but if running `make sdk-release` this directory is first purged). This would remove the need for manually defined paths, and allow us to create a more automated build process.

To build BigMPI we next run the following commands from the `${QHIPSTER_ROOT}` directory:
```bash
mkdir ${QHIPSTER_ROOT}/install
cd ${QHIPSTER_ROOT}/util/BigMPI/
```

For a static library of `libbigmpi.a` we can use the autotools build steps, which are slightly modified from the ones given by the [BigMPI documentation](https://github.com/intel/Intel-QS/blob/consistent-naming/util/BigMPI/INSTALL.md). We install the BigMPI library into the Intel-QS build directory.
```bash
./autogen.sh;
./configure CC=mpiicc --prefix=${QHIPSTER_ROOT}/install;
make && make install;
```

For a dynamic library of `libbigmpi.so` we can use the CMake build process and perform the installation into the same directory as previously (we can also build a static library using CMake, but the default is shared).
```bash
CC=mpiicc CXX=mpiicpc cmake ..
```
However, as this seems to call the autotools pipeline behind the scenes, it can be easier to just use the earlier static build instructions.


After building and installing the library, we return to `${QHIPSTER_ROOT}`, clean the previous build, and make the library again specifying the BigMPI location (which again, would be best if it was automatically entered as part of the previous build process):
```bash
cd ${QHIPSTER_ROOT}
make clean
make all BIGMPI_LIB=${QHIPSTER_ROOT}/install/lib/libbigmpi.a BIGMPI_INC=-I${QHIPSTER_ROOT}/install/include
```

This will generate all of the required files to perform an install of the SDK. However, this step begins by removing the pre-existing build directory, and as a result all of the existing BigMPI-built entities. However, we can simply generate the required library and headers into `build` with
```bash
make sdk-release BIGMPI_LIB=${QHIPSTER_ROOT}/install/lib/libbigmpi.a BIGMPI_INC=-I${QHIPSTER_ROOT}/install/include
```

# Using the SDK
When building and installing the qHiPSTER SDK, examining the makefile suggests this is performed using `make sdk-release`. However, due to the lack of a `LICENSE.txt` file, this fails. This would be rectified by adding the appropriate license file to the `${QHIPSTER_ROOT}` directory. For the sake of our tests, we simply `touch LICENSE.txt`.

Next, upon having an installed SDK, we aimed to build a simple application to implement a controlled swap operation. For this, including `qureg.hpp` is necessary. However, doing this reveals that the `${QHIPSTER_ROOT}/build/include` directory is missing some header files, specifically the `NoisyQureg.hpp` file, which `qureg.hpp` depends upon. Consider modifying `makefile` and the `sdk-copy-sources` macro to install all headers from `./qureg/` rather than a select few would correct this.

# Some additional notes
These are some suggestions based on our work so far.

 - The use of our predefined `install` and Intel-QS `build` directories gives us two separate paths to includes and libraries; it may be best to redefine the build system such that `make sdk-release` does not remove any files, and additionally, the use of a `--prefix` flag for controlled installation. This would allow us to essentially enable a single directory for installing all of the software (`libbigmpi.a, qhipster.a`, etc.). 

 - The renaming of `qhispter.a` to `libqhipster.a` may also ease use with other builds. As it follows the standard Unix library naming conventions, it would allow use of the linker `-l` to search what is on path without being explicit.
 
 - For the 'consistent-naming' branch of Intel-QS, the test examples in the Intel-QS/test directory only work for a single process since they are built without the compiler flag -DNOREPA_HAS_MPI being set, disabling MPI. Unless there is a reason not to, it might be worthwhile setting this flag.
 