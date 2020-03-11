# QNLP - Quantum Natural Language Processing

In this software suite we have developed a workflow to implement a hybridised classical-quantum model for representing language and representing sentence meanings in quantum states.

## The model

This work is based upon and inspired by the **DisCo** (also known as **DisCoCat**) formalism of Coecke *et al.* [1,2,3], wherein both compositional and distributional sentence structure and information is used to represent meaning. Following the work on Zeng and Coecke [3], we aimed to implement a modified approach to implement this model for quantum processors. Further details will follow shortly in the upcoming paper.

## Requirements

To build and run this software suite it is recommended to have the Intel compilers and Intel MPI libraries. We make use of the Intel Quantum Simulaton (Intel-QS, qHiPSTER) [4,5] as the underlying simulator for our methods. If not available, the build can use GCC/Clang, provided the toolchain supports C++14 at a minimum (C++17 prefered). In addition, an MPI library is required. We have successfully used both MPICH and OpenMPI with non-Intel compiler builds. 

This suite is primarily developed for Linux HPC systems, though it may also run on laptops/desktops. MacOS can be used, but some additional work is necessary. 

## Documentation and examples

All documentation for this project is available at https://ichec.github.io/qnlp

- [Installation](https://ichec.github.io/qnlp/install) and [build](Build.md)
- [API](https://ichec.github.io/qnlp/docs)
- [Examples](https://ichec.github.io/qnlp/examples)

Example runnable scripts are available in the `modules/py/scripts` directory. C++ demos and tests are available under `demos` and `modules/tests` respectively. Jupyter notebooks are available at `modules/py/nb`.

---

- [1]: Stephen Clark, Bob Coecke and Mehrnoosh Sadrzadeh, *A Compositional Distributional Model of Meaning*, Proceedings of the Second Quantum Interaction Symposium, 2008.
- [2]: Bob Coecke, Mehrnoosh Sadrzadeh and Stephen Clark, *Mathematical Foundations of a Compositional Distributional Model of Meaning*, Special issue of Linguistic Analysis, 2010. [arXiv:1003.4394](https://arxiv.org/abs/1003.4394)
- [3]: William Zeng and Bob Coecke, *Quantum Algorithms for Compositional Natural Language Processing*, Proceedings of SLPCS, 2016. [arXiv:1608.01406](https://arxiv.org/pdf/1608.01406.pdf)
- [4]: Mikhail Smelyanskiy, Nicolas P. D. Sawaya, Alán Aspuru-Guzik. *qHiPSTER: The Quantum High Performance Software Testing Environment*, [arXiv:1601.07195](https://arxiv.org/abs/1601.07195)
- [5]: Gian Giacomo Guerreschi, Justin Hogaboam, Fabio Baruffa, Nicolas P. D. Sawaya, *Intel Quantum Simulator: A cloud-ready high-performance simulator of quantum circuits*. [arXiv:2001.10554](https://arxiv.org/abs/2001.10554)
