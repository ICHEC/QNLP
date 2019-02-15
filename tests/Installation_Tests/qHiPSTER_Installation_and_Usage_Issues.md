# Installation and Usage Issues of qHiPSTER using BigMPI

For further details and work-arounds of the issues with installation procedure and usage of qHiPSTER see the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` (https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/Installation_Tests/qHiPSTER_installation_testing.md and https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/qHiPSTER_build_process.md respectively)

## Installation Process

- Fabio recommended using the more up to date `consistent-naming` branch of the repository instead of using the default `master` branch.
    - Issues: 
        - `master` branch appears to be relatively outdated compared to consistent-naming` branch.
    - Consequences: 
        - Naming conventions change between the two branches, therefore it would make sense to have the more updated one being used rather than requiring users to change their source code.
    - Consider pushing the `consistent-naming` branch to `master`.

- The installation process of the SDK fails if one follows the standard installation step (```make sdk-release```). See the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` files for explicit details.
    - Issues:
        - Does not build.
        - Assumes BigMPI has been pre-installed on machine (BigMPI was provided with installation but not built).
            - Requires user to manually change paths in `make.inc` once BigMPI has been manually installed.
        - Does not work when built due to missing header file (`NoisyQureg.hpp`).
    - Consequences:
        - Installation process is much more involved than it should be and requires a lot of digging around to get working. This makes a process that should be completed by executing one or two commands into a more lengthy procedure to find and resolve the errors.
    

## Using qHiPSTER with BigMPI

- BigMPI does not appear to work for the provided test cases (example `Intel-QS/tests/qft_test.cpp`) despite qHiPSTER being built with BigMPI.
    - Issues:
        - Does not work (reports MPI communication error for experiments with larger problem sizes).
    - Possible Cause of Issue: 
        - Only `MPIX_Sendrecv_x` and `MPI_Allreduce_x` have been implemented as BigMPI calls for their respective MPI calls. We are not sure if they are the only required MPI routines that should use BigMPI or whether other BigMPI routines are necessary but not yet implemented.
    - Consequences:
        - This is a significant problem which will result in a roadblock for the project, preventing scaling of our problem to larger sizes.
    
- There is no validation of the library using unit tests.
    - Issues:
        - Assumes that the routines work without rigorous testing.
    - Consequences:
        - User is not ensured that the routines work correctly and can only assume so unless they conduct their own testing.
    - It would be nice and good practise to provide unit tests to assure the user that the routines work as expected.

- We came across some odd comments in the `master` branch but I think most if not all of them were removed in the `consistent-naming` branch. Might be worth taking another look at, but is a very minor issue.


- There is no documentation for using the SDK.
    - Issues:
        - As stated.
    - Consequences:
        - Development using the SDK can be very touch and go. As in, how does the developer know that the way they have used the routines is the intended use of the routines.
        - Makes using qHiPSTER more inaccessible for the developer.
        - In terms of getting qHiPSTER working with MPI in general and with BigMPI, a number of precompiler flags are required to be specified. There is no way for the developer to know what these flags are apart from going through the samples and the `Makefile`s. It would be very useful to have this stated in some documentation, as well as the purpose of each flag (examples `-DBIGMPI`,`-DOPENQU_HAVE_MPI` and `-DINTELQS_HAS_MPI`).
        - Examples requiring the `-DNOREPA_HAS_MPI` flag to enable MPI do not provide information about or set by default this flag. It is ambiguous whether there is a reason why MPI was disabled in the example programs in `Intel-QS/tests/` for the `consistent-naming` branch of qHiPSTER.
    - Following from this, for the project it would be useful to know the correct and intended way for the user to define their own gates. We have provided our example of using qHiPSTER to implement a 3 qubit controlled swap (see `ApplyControlledSwap` function in `intel-qnlp/tests/test_control_swap.cpp`). Would this be the intended way qHiPSTER should be used to do this, or is there a better way? Documentation to help the user make these decisions would be very useful.


## Summary of Main Issues
The above information was provided to give feedback for issues we have come across when using qHiPSTER so far from an aspect of usability in general. In terms of our project, the only main issue is that BigMPI is not working with the qHiPSTER library. While documentation on using qHiPSTER as a SDK would be very helpful to have, it is clear that creating useful documentation takes a good bit of time, but it is something that would really help other users in the future.