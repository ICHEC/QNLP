#include "pybind11/pybind11.h"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "pybind11/complex.h"
#include "pybind11/stl.h"

namespace py = pybind11;
using namespace QNLP;
using DCM = openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u>;

class IntelSimMixin : public IntelSimulator{
    public:
    IntelSimMixin(int numQubits, bool useFusion=false) : IntelSimulator(numQubits,  useFusion) {}
    ~IntelSimMixin(){}

    void applyGateNCU_linear(const DCM& U, std::size_t minIdx, std::size_t maxIdx, std::size_t target, std::string label = "U"){
        this->applyGateNCU(U, minIdx, maxIdx, target, label);
    }
    void applyGateNCU_nonlinear(const DCM& U, std::vector<std::size_t>& ctrl_indices, std::size_t target, std::string label = "U"){
        this->applyGateNCU(U, ctrl_indices, target, label);
    }
};

template <class SimulatorType>
void intel_simulator_binding(py::module &m){
    py::class_<SimulatorType>(m, "IntelSimulator")
        .def(py::init<const std::size_t &, const bool &>())
        .def("getGateX", &SimulatorType::getGateX, py::return_value_policy::reference)
        .def("getGateY", &SimulatorType::getGateY, py::return_value_policy::reference)
        .def("getGateZ", &SimulatorType::getGateZ, py::return_value_policy::reference)
        .def("getGateI", &SimulatorType::getGateI, py::return_value_policy::reference)
        .def("getGateH", &SimulatorType::getGateH, py::return_value_policy::reference)
        .def("applyGateX", &SimulatorType::applyGateX)
        .def("applyGateY", &SimulatorType::applyGateY)
        .def("applyGateZ", &SimulatorType::applyGateZ)
        .def("applyGateH", &SimulatorType::applyGateH)
        .def("applyGateI", &SimulatorType::applyGateH)
        .def("applyGateSqrtX", &SimulatorType::applyGateSqrtX)
        .def("applyGateRotX", &SimulatorType::applyGateRotX)
        .def("applyGateRotY", &SimulatorType::applyGateRotY)
        .def("applyGateRotZ", &SimulatorType::applyGateRotZ)
        .def("applyGateCRotX", &SimulatorType::applyGateCRotX)
        .def("applyGateCRotY", &SimulatorType::applyGateCRotY)
        .def("applyGateCRotZ", &SimulatorType::applyGateCRotZ)
        .def("applyGateU", &SimulatorType::applyGateU)
        .def("applyGateCU", &SimulatorType::applyGateCU)
        .def("applyGateSwap", &SimulatorType::applyGateSwap)
        .def("applyGateSqrtSwap", &SimulatorType::applyGateSqrtSwap)
        .def("applyGatePhaseShift", &SimulatorType::applyGatePhaseShift)
        .def("applyGateCPhaseShift", &SimulatorType::applyGateCPhaseShift)
        .def("applyGateCCX", &SimulatorType::applyGateCCX)
        .def("applyGateCSwap", &SimulatorType::applyGateCSwap)
        .def("getNumQubits", &SimulatorType::getNumQubits)
        .def("applyQFT", &SimulatorType::applyQFT)
        .def("applyIQFT", &SimulatorType::applyIQFT)
        .def("applyDiffusion", &SimulatorType::applyDiffusion)
        .def("encodeToRegister", &SimulatorType::encodeToRegister)
        .def("encodeBinToSuperpos_unique", &SimulatorType::encodeBinToSuperpos_unique)
        .def("encodeBinToSuperpos", &SimulatorType::encodeBinToSuperpos)
        .def("applyHammingDistance", &SimulatorType::applyHammingDistance)
        .def("applyHammingDistanceRotY", &SimulatorType::applyHammingDistanceRotY)
        .def("applyMeasurement", &SimulatorType::applyMeasurement)
        .def("applyMeasurementToRegister", &SimulatorType::applyMeasurementToRegister)
        .def("collapseToBasisZ", &SimulatorType::collapseToBasisZ)
        .def("initRegister", &SimulatorType::initRegister)
        .def("printStates", &SimulatorType::PrintStates)
        .def("applyGateNCU", &SimulatorType::applyGateNCU_linear)
        .def("applyGateNCU", &SimulatorType::applyGateNCU_nonlinear)
        .def("applyOraclePhase", &SimulatorType::applyOraclePhase);
/*
        .def("applyGateNCU", py::overload_cast<DCM, std::size_t, std::size_t, std::size_t, std::string>(&SimulatorType::applyGateNCU), "Adjacent control line NCU")
        .def("applyGateNCU", py::overload_cast<DCM, std::vector<std::size_t>, std::size_t, std::string>(&SimulatorType::applyGateNCU), "Non-adjacent control line NCU")
        .def("applyOracleU", &SimulatorType::applyOracleU)
        .def("adjointMatrix", &SimulatorType::adjointMatrix)
        .def("matrixSqrt", &SimulatorType::matrixSqrt)
        .def("getQubitRegister", &SimulatorType::getQubitRegister)        
*/

    //TinyMatrix
    py::class_<DCM>(m, "DCMatrix")
        .def(py::init<>())
        .def("__getitem__", 
            [](const DCM &s, std::size_t i, std::size_t j) {
                if (i >= 2 || j >= 2) 
                    throw py::index_error();
                return s(i,j);
            });

    /** WIP: NumPy buffer interface fo data access
      py::class_<openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u>>(m, "Matrix", py::buffer_protocol())
      .def_buffer([](openqu::TinyMatrix<std::complex<double>, 2u, 2u, 32u> &m) -> py::buffer_info {
        return py::buffer_info(
            m.data(),                               // Pointer to buffer
            sizeof(float),                          // Size of one scalar 
            py::format_descriptor<float>::format(), // Python struct-style format descriptor 
            2,                                      // Number of dimensions 
            { m.rows(), m.cols() },                 // Buffer dimensions 
            {   sizeof(float) * m.rows(),             // Strides (in bytes) for each index 
                sizeof(float) 
            }
        );
      });
     */
}

PYBIND11_MODULE(PythonSimulator, m){
    intel_simulator_binding<IntelSimMixin>(m);
}
