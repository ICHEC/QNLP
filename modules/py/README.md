# Python functionality

This directory contains all of the Python functionality for the QNLP project.

- bindings: contains the pybind11 binding code and packaging for the wrapped C++ functionality.
- nb: contains Jupyter notebooks demonstrating the functionality of the QNLP work.
- pkgs: Python-only pacmages for the QNLP routines.
- scripts: Runnable Python scripts using the QNLP Python environment

## QNLP package

The QNLP Python package contains many classes, methods, and modules
to assist with the pre-processing of corpus data, as we all subsequent
analysis.

The structure of the package is as follows:

- encoding: module that encapsulates the chosen encoding schema for the QNLP state representations
- io: \[deprecated\] functionality to read data exported using the C++ SQLite functions.
- proc: functionality to tokenise, tag, and analyse corpus data in both pre and post-processing.
- tagging: \[deprecated\] simple functionality for representing type information

## CircuitPrinter

This package and runnable script takes the output files from a QNLP build with `-DENABLE_LOGGING=1` set. 
Assuming a LaTeX installation, the generated output will compile to a `quantikz`-enabled circuit diagram.

## PyQNLPSimulator

This package holds all C++-backed methods, and Numpy conversions. We use this package to create quantum simulators,
and all available methods allow interaction with the subsequent state.

