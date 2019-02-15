# Instllation and Usage Issues of qHiPSTER using BigMPI

## Installation Process

For further details and work-arounds of the issues with installation procedure see the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` report (https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/Installation_Tests/qHiPSTER_installation_testing.md and https://git.ichec.ie/intel-qnlp/intel-qnlp/blob/master/tests/qHiPSTER_build_process.md respectively)

- Fabio recommended using the more up to date `consistent-naming` branch of the repository instead of using the default `master` branch
    - Issue: `master` branch appears t be relatively outdated compared to consistent-naming` branch
    - Consider pushing the `consistent-naming` branch to `master`
    - Consequences: Naming conventions change between the two branches, therefore it would make sense to have the more updated one being used rather than requiring users to change their source code

- The installation process of the SDK fails if one follows the standard installation step (```make sdk-release```). See the `qHiPSTER_installation_testing.md` and `qHiPSTER_build_process.md` files above for explicit details
    - Issues:
        - Does not build
        - Assumes BigMPI has been pre-installed on machine (BigMPI was provided with installation but not built)
            - Requires user to manually change paths in `make.inc` once BigMPI has been manually installed
        - Does not work when built due to missing header file (`NoisyQureg.hpp`)
    - Consequences:
        - Installation process is much more involved than it should be and requires a lot of digging around to get working. This makes a process that should be completed by executing one or two commands into more lengthy procedure to find the errors.
    
    

    
