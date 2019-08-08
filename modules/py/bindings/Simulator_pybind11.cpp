#include "pybind11/pybind11.h"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "pybind11/complex.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace QNLP;
using DCM = openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u>;

template <class SimulatorType>
void intel_simulator_binding(py::module &m){
    //IntelSimulator sim(8);
    py::class_<SimulatorType>(m, "IntelSimulator")
        .def(py::init<const std::size_t &>())
        .def("getGateX", &SimulatorType::getGateX);

    //TinyMatrix
    py::class_<DCM>(m, "DCMatrix")
        .def(py::init<>())
        .def("__getitem__", [](const DCM &s, std::size_t i, std::size_t j) {
            if (i >= 2 || j >= 2) throw py::index_error();
                return s(i,j);
            });


    /** NumPy buffer interface
      py::class_<openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u>>(m, "Matrix", py::buffer_protocol())
      .def_buffer([](openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u> &m) -> py::buffer_info {
      return py::buffer_info(
      m.data(),                               // Pointer to buffer
      sizeof(float),                          // Size of one scalar 
      py::format_descriptor<float>::format(), // Python struct-style format descriptor 
      2,                                      // Number of dimensions 
      { m.rows(), m.cols() },                 // Buffer dimensions 
      { sizeof(float) * m.rows(),             // Strides (in bytes) for each index 
      sizeof(float) }
      );
      });
     */
}

PYBIND11_MODULE(PythonSimulator, m){
    intel_simulator_binding<IntelSimulator>(m);
}
