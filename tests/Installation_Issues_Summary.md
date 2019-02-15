# Instllation and Usage Issues of qHiPSTER using BigMPI

For further details and work-arounds of the issues with installation procedure and usage of qHiPSTER see the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` report (https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/Installation_Tests/qHiPSTER_installation_testing.md and https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/qHiPSTER_build_process.md respectively)

## Installation Process



- Fabio recommended using the more up to date `consistent-naming` branch of the repository instead of using the default `master` branch.
    - Issue: `master` branch appears t be relatively outdated compared to consistent-naming` branch
    - Consider pushing the `consistent-naming` branch to `master`
    - Consequences: Naming conventions change between the two branches, therefore it would make sense to have the more updated one being used rather than requiring users to change their source code

- The installation process of the SDK fails if one follows the standard installation step (```make sdk-release```). See the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` files above for explicit details.
    - Issues:
        - Does not build
        - Assumes BigMPI has been pre-installed on machine (BigMPI was provided with installation but not built)
            - Requires user to manually change paths in `make.inc` once BigMPI has been manually installed
        - Does not work when built due to missing header file (`NoisyQureg.hpp`)
    - Consequences:
        - Installation process is much more involved than it should be and requires a lot of digging around to get working. This makes a process that should be completed by executing one or two commands into more lengthy procedure to find the errors.
    

## Using qHiPSTER with BigMPI

- BigMPI does not appear to work for the provided test cases (example `Intel-QS/tests/qft_test.cpp`).
    - Issues:
        - Does not work (reports MPI communication error for experiments with larger problem sizes)
    - Possible Cause of Issue: 
        - Only `MPIX_Sendrecv_x` and `MPI_Allreduce_x` have been implemented as BigMPI calls for their respective MPI calls. We are not sure if they are the only required MPI routines that should use BigMPI or whether other BigMPI routines are necessary but not yet implemented.
    - Consequences:
        - This is a significant problem which will result in a roadblock for the project, preventing scaling of our problem to larger sizes
    
- There is no validation of the library using unit tests
    - Issues:
        - Assumes that the routines work without test
    - It would be nice and good practise to provide unit tests to assure the user that the routines work as expected

- We came across some odd comments in the `master` branch but I think most if not all of them were removed in the `consistent-naming` branch. Might be worth taking another look at, but is a very minor issue.
