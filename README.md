# QNLP - Quantum Natural Language Processing
<img src="https://travis-ci.com/ICHEC/QNLP.svg?branch=master">

In this software suite we have developed a workflow to implement a hybridised classical-quantum model for representing language and representing sentence meanings in quantum states [1].

## The model

This work is based upon and inspired by the **DisCo** (also known as **DisCoCat**) formalism of Coecke *et al.* [2,3,4], wherein both compositional and distributional sentence structure and information is used to represent meaning. Following the work on Zeng and Coecke [4], we aimed to implement a modified approach to implement this model for quantum processors. Further details will follow shortly in the upcoming paper.

## Requirements

To build and run this software suite it is recommended to have the Intel compilers and Intel MPI libraries. We make use of the Intel Quantum Simulator (Intel-QS, qHiPSTER) [5,6] as the underlying simulator for our methods. If not available, the build can use GCC/Clang, provided the toolchain supports C++14 at a minimum (C++17 prefered). In addition, an MPI library is required. We have successfully used both MPICH and OpenMPI with non-Intel compiler builds. 

This suite is primarily developed for Linux HPC systems, though it may also run on laptops/desktops. MacOS can be used, but some additional work is necessary. 

## Documentation and examples

All documentation for this project is available at https://ichec.github.io/QNLP

- [Installation](https://ichec.github.io/QNLP/install) and [build](Build.md)
- [API](https://ichec.github.io/QNLP/docs)
- [Examples](https://ichec.github.io/QNLP/examples)

Example runnable scripts are available in the `modules/py/scripts` directory. C++ demos and tests are available under `demos` and `modules/tests` respectively. Jupyter notebooks are available at `modules/py/nb`.

## Referencing
If using this work in any way as part of your research, please feel free to cite us as:
"***A hybrid classical-quantum workflow for natural language processing***" by O'Riordan et al, *Mach. Learn.: Sci. Technol.* (2020). DOI: https://doi.org/10.1088/2632-2153/abbd2e

---

- [1]: Lee J. O'Riordan, Myles Doyle, Fabio Baruffa, Venkatesh Kannan, *A hybrid classical-quantum workflow for natural language processing*, IOP Machine Learning: Science and Technology (2020). DOI: https://doi.org/10.1088/2632-2153/abbd2e
- [2]: Stephen Clark, Bob Coecke and Mehrnoosh Sadrzadeh, *A Compositional Distributional Model of Meaning*, Proceedings of the Second Quantum Interaction Symposium, 2008.
- [3]: Bob Coecke, Mehrnoosh Sadrzadeh and Stephen Clark, *Mathematical Foundations of a Compositional Distributional Model of Meaning*, Special issue of Linguistic Analysis, 2010. [arXiv:1003.4394](https://arxiv.org/abs/1003.4394)
- [4]: William Zeng and Bob Coecke, *Quantum Algorithms for Compositional Natural Language Processing*, Proceedings of SLPCS, 2016. [arXiv:1608.01406](https://arxiv.org/pdf/1608.01406.pdf)
- [5]: Mikhail Smelyanskiy, Nicolas P. D. Sawaya, Alán Aspuru-Guzik. *qHiPSTER: The Quantum High Performance Software Testing Environment*, [arXiv:1601.07195](https://arxiv.org/abs/1601.07195)
- [6]: Gian Giacomo Guerreschi, Justin Hogaboam, Fabio Baruffa, Nicolas P. D. Sawaya, *Intel Quantum Simulator: A cloud-ready high-performance simulator of quantum circuits*. [arXiv:2001.10554](https://arxiv.org/abs/2001.10554)
