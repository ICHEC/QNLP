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

## Build from Makefile
```
make
```
