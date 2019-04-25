# Project modules

This directory contains the individual modules that make up the QNLP project.

To add a new module to the project, consider adding a directory here with the name representing the module. Inside, there should be a collection of source and headers, along with a `README.md` to describe the module's functionality, and a `CMakeLists.txt` to allow the module to be built with the top level project. This CMakeLists.txt should be included in the above directory (i.e. this one). Nesting of directories is allowed, provided the previous rule is followed.