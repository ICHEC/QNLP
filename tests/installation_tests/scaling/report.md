# Testing QHiPSTER using the `qfft_test.cpp` program from the installation located at `Intel-QS/tests`

`qfft_test.cpp` was copied to this location and built using the `CMakeLists.txt` that was created independently from the Intel-QS installation. BigMPI was used in the build of this program. It worked successfully for both the `master` and the a `consistent-build` branches. Weak and strong scaling was completed on up to the maximum possible problem size permitted, due to the number of available nodes in Kay's DevQ. Note, that Kay permits a maximum of 80 nodes in a standard allocation but the number or processes is required to be a power of 2. Therefore, a maximum of only 64 nodes could be used due to the memory constraint resulting in only two processes per node working for larger problem sizes (35 qubits).

The implementation worked for up to and including 35 qubits for both single and double precision. The program failed for larger than 35 qubits due to communication errors
