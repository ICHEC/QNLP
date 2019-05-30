# Building Intel-QS on MacOS
While Intel-QS is not officially supported on MacOS, it can be beneficial to have
an offline (i.e. no-cluster) build environment. The following steps allowed for
the build of the library `qHiPSTER.a` on MacOS Mojave (10.14.x).

It should be possible to build the QNLP project against this library. If not, I will
continue to update this guide.

# Install development headers
MacOS Mojave for some reason does not give you a `/usr/include`, and all associated headers. The following command 
requires that you have XCode installed, and should fix the issue:
```bash
xcode-select --install
sudo installer -pkg /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg -target /
```

# Install GCC 
Firstly, as some of the Intel-QS code has GCC specific support (and nothing is mentioned
for clang), we opted to install the latest GCC compiler (GCC 9.1 as of now). This was 
enabled doing the following:

```bash
curl -O ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-9.1.0/gcc-9.1.0.tar.xz
tar xvf gcc-9.1.0.tar.xz
cd gcc-9.1.0
./contrib/download_prerequisites
./configure --prefix=/opt/gcc/gcc91 --enable-languages=c,c++ --disable-multilib --enable-threads=posix
make -j8 && make install
```
where we have built the compiler to run from the directory `/opt/gcc/gcc91`.

# Install MPI (OpenMPI)
For the next step, we require an MPI installation. I have opted to use OpenMPI, but MPICH should work
just fine also (bare in mind, some updates to the MPI standard required minor code edits, and so OpenMPI 4.x is assumed). 
For this, we build the MPI libraries using our previously built GCC:

```bash
curl -O https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.1.tar.gz
tar xvf ./openmpi-4.0.1.tar.gz
cd openmpi-4.0.1
./configure --prefix=/opt/gcc/gcc91 CC=/opt/gcc/gcc91/bin/gcc CXX=/opt/gcc/gcc91/bin/g++ --enable-mpi-cxx --disable-mpi-fortran
make -j8 && make install
```
where we have installed the libraries into the same directory as the compiler.

# Install MKL (using conda)
Next, using the conda environment setup by intel-qnlp (`setup_env.sh ${PWD}` followed by `source load_env.sh`)
we can have access to the required MKL libraries. We must first install the MKL headers by running:

```bash
source <PATH_TO_QNLP>/load_env.sh
conda install mkl-include
```

# Building Intel-QS
As not all code is designed to run with modern GCC, and some expectations exist for the Intel compiler, a few
edits are necessary to ensure Intel-QS builds correctly. These edits are included with Intel-QNLP as the file
`Intel-QS_MacOS.patch` (located in this directory).

```bash
cd Intel-QS
git checkout consistent-naming
git apply <PATH_TO_QNLP>/misc/Intel-QS_MacOS.patch
export MKLROOT=${CONDA_PREFIX}
PATH=/opt/gcc/gcc91/bin:$PATH \
LD_LIBRARY_PATH=/opt/gcc/gcc91/lib:${CONDA_PREFIX}/lib:${LD_LIBRARY_PATH} \
DYLD_LIBRARY_PATH=/opt/gcc/gcc91/lib:${CONDA_PREFIX}/lib:${DYLD_LIBRARY_PATH} \
make
```

The above paths are overridden to ensure we are using the correct compiler and library directories. The library
should be built as `qureg/qHiPSTER.a` if all things went well.


# Using Environment Modules to set Intel-QS environment
The TCL script `intel_qs-1.0` provided can be used as an Environments Module file to set the Intel-QS environment variables. For this to work, it is assumed that Environment Modules is installed on your local system. This can be easily installed using Spack (installation detailed here: https://spack.readthedocs.io/en/latest/getting_started.html) under the Environment Modules heading (also loacated in previous link) or by installing from source or another package manager.

The user can decide where the module file will be placed, however for ease of use it was placed in the path `/opt/modulefiles/libraries`. `intel_qs-1.0` must now be specified for your installation of the above software. Thus, the local variable 'topdir' should be set to the path `<path-to-Intel-QS>/Intel-QS`. From the above instructions, it is assumed that the CC and CXX compilers are located in the path `/opt/gcc/gcc91/bin/`. If this is not the case, each of the respective paths should be set appropriately.

The `/opt/modulefiles` directory must now be made visible to Environment Modules. This can be done using
```bash
module use /opt/modulefiles/
```
This command can also be added to your `.bash_rc` so that you do not need to re-run this command each time you want to load a module you created.

The Intel-QS environment variables `QHIPSTER_DIR`, `QHIPSTER_DIR_LIB`, `QHIPSTER_DIR_INC` and the `CC` and `CXX` compilers can now be set using;
```bash
module load libraries/intel_qs-1.0
```


