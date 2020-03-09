# Testing QNLP build

The QNLP suite of tools and libraries uses the (catch2)[https://github.com/catchorg/catch2] testing framework.

For every module in `<QNLP_ROOT>/modules` an appropriate set of unit & integration tests should also be specified in a file named `test_<module-name>.cpp`.
The test target should be added to the module's `CMakeLists.txt` with a `$CMAKE_TESTING_ENABLED` variable guard. Each module tests are built as a library,
and the `<QNLP_ROOT>/modules/CMakeLists.txt` is updated to link againt the `tests` target.

This ensures that all tests are included in the resulting `tests` binary file, and are selectable at run-time.

Tests for the currently utilised modules are all expected to pass, with additional functionality still a work-in-progress.
For the current release, the following tests successfully pass on Linux and MacOS:

- `[simulator]` : basic functionality of the quantum simulator with the Intel-QS backend (`modules/simulator`)
- `[hammingrot]`: hamming distance weighting of state amplitudes  (`modules/hamming`)
- `[ncu]`: n-controlled unitary (`modules/gate_ops/ncu`)
- `[encode]`: encoding of bitstrings into the quantum register (`modules/encoding`)
- `[diffusion]`: the diffusion operator (`modules/gate_ops/diffusion`)
- `[oracle]`: integer bit-wise phase oracle (`modules/gate_ops/oracle`)

Additional tests are included within the binary, but not all are expected to pass. As such, it is best to test the above modules individually as a comma-separated list of the test labels. For example

```bash
source ./load_env.sh
./build/modules/test/tests "[simulator],[ncu]"
```
to run the simulator and n-controlled unitary tests.

## Note:
The tests are not generally MPI-aware, and may fail if launched as such. It is recommended to run these tests with an OpenMP build only, as this will offer better performance.

