# Build Steps for `intel-qnlp` on Kay

## Load Intel Compiler and Cmake3
Load modules for Intel-2018-u3 and Cmake3.

```
module load intel cmake3
```

## Setup Intel-QS environment variables
Export paths to environment variables to enable the Intel-QS environment.
```
export QHIPSTER_DIR=<PATH-TO-BUILD-DIR-OF-INTEL_QS>
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QHIPSTER_DIR/lib
export QHIPSTER_DIR_INC=$LD_LIBRARY_PATH:$QHIPSTER_DIR/include
export QHIPSTER_DIR_LIB=$LD_LIBRARY_PATH:$QHIPSTER_DIR/lib/intel64
```

Note it would be convenient to specify the above paths in a EnvironmentModules file which can easily be loaded and unloaded. See `intel-qnlp/misc/ intel_qs-1.0` for an example boiler plate file to do this.

## Build environment configuration
Run script which pulls down Python and libsqllite3 packages, and then builds them.
```
./setup_env.sh
```

## Set environment configuration
Set environment variables for libsqllite3and Python.

```
source load_env.sh
```

## Cmake build steps
Run Cmake to create the Maklefile using the defined environment.
```
cd build
CC=mpiicc CXX=mpiicpc cmake ..
```

Note: to build on Mac OS-Mojave, the following command should be run to create the Makefile provided the softaware including Intel-QS was built appropriately for the target environment.
```
cd build
PATH=/opt/gcc/gcc91/bin:$PATH CC=mpicc CXX=mpicxx cmake ..
```

## Build from Makefile
```
make
```

## Note: Running on Mac OS
To run an executable that uses the `intel-qnlp` library, the command must be prepended with the `DYLD_LIBRARY_PATH` path as follows.
```
DYLD_LIBRARY_PATH=/opt/gcc/gcc91/lib:${CONDA_PREFIX}/lib:${DYLD_LIBRARY_PATH} <PATH-TO-EXECUTABLE>/<EXECUTABLE>
```
