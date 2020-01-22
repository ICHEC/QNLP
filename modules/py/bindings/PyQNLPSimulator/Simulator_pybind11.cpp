#include "pybind11/pybind11.h"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "pybind11/complex.h"
#include "pybind11/stl.h"
#include <pybind11/numpy.h>
#include <mpi.h>

#define PYBIND11_EXPORT __attribute__ ((visibility("default")))

namespace py = pybind11;
using namespace QNLP;
using DCM = qhipster::TinyMatrix<std::complex<double>, 2u, 2u, 32u>;

class IntelSimPy : public IntelSimulator{
    public:

    IntelSimPy(int numQubits, bool useFusion=false) : IntelSimulator(numQubits,  useFusion) { }
    IntelSimPy(std::unique_ptr<IntelSimulator, std::default_delete<IntelSimulator> > iSim) : IntelSimulator(iSim->getNumQubits(), false) {}
    ~IntelSimPy(){}

    void applyGateNCU_nonlinear(const DCM& U, std::vector<std::size_t>& ctrl_indices, std::size_t target, std::string label){
        this->applyGateNCU(U, ctrl_indices, {}, target, label);
    }
    void applyGateNCU_5CX_Opt(const DCM& U, std::vector<std::size_t>& ctrl_indices, std::vector<std::size_t>& aux_indices, std::size_t target, std::string label){
        this->applyGateNCU(U, ctrl_indices, aux_indices, target, label);
    }
    void applyOracle_U(std::size_t bit_pattern, const DCM& U, std::vector<std::size_t>& ctrl_indices, std::size_t target, std::string label){
        this->applyOracleU( bit_pattern, ctrl_indices, target, U, label);
    }
};

template <class SimulatorType>
void intel_simulator_binding(py::module &m){
    // Define various trivial types to allow simpler overload resolution:
    py::module m_tag = m.def_submodule("tag");
#define MAKE_TAG_TYPE(Name) \
    struct Name##_tag {}; \
    py::class_<Name##_tag>(m_tag, #Name "_tag").def(py::init<>()); \
    m_tag.attr(#Name) = py::cast(Name##_tag{})
    MAKE_TAG_TYPE(pointer);
    MAKE_TAG_TYPE(unique_ptr);
    MAKE_TAG_TYPE(move);
    MAKE_TAG_TYPE(shared_ptr);
    MAKE_TAG_TYPE(derived);
    MAKE_TAG_TYPE(TF4);
    MAKE_TAG_TYPE(TF5);
    MAKE_TAG_TYPE(null_ptr);
    MAKE_TAG_TYPE(base);
    MAKE_TAG_TYPE(invalid_base);
    MAKE_TAG_TYPE(alias);
    MAKE_TAG_TYPE(unaliasable);
    MAKE_TAG_TYPE(mixed);

    py::class_<SimulatorType>(m, "PyQNLPSimulator")
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
        .def("applyGateCX", &SimulatorType::applyGateCX)
        .def("applyGateCCX", &SimulatorType::applyGateCCX)
        .def("applyGateCSwap", &SimulatorType::applyGateCSwap)
        .def("getNumQubits", &SimulatorType::getNumQubits)
        .def("applyQFT", &SimulatorType::applyQFT)
        .def("applyIQFT", &SimulatorType::applyIQFT)
        .def("applyDiffusion", &SimulatorType::applyDiffusion)
        .def("encodeToRegister", &SimulatorType::encodeToRegister)
        .def("encodeBinToSuperpos_unique", &SimulatorType::encodeBinToSuperpos_unique)
        .def("applyHammingDistanceRotY", &SimulatorType::applyHammingDistanceRotY)
        .def("applyMeasurement", &SimulatorType::applyMeasurement)
        .def("applyMeasurementToRegister", &SimulatorType::applyMeasurementToRegister)
        .def("collapseToBasisZ", &SimulatorType::collapseToBasisZ)
        .def("initRegister", &SimulatorType::initRegister)
        .def("printStates", &SimulatorType::PrintStates)
        .def("applyGateNCU", &SimulatorType::applyGateNCU_nonlinear)
        .def("applyGateNCU", &SimulatorType::applyGateNCU_5CX_Opt)
        .def("subReg", &SimulatorType::subReg)
        .def("sumReg", &SimulatorType::sumReg)
        .def("applyOracleU", &SimulatorType::applyOracle_U)
        .def("applyOraclePhase", &SimulatorType::applyOraclePhase);
/*
        .def("adjointMatrix", &SimulatorType::adjointMatrix)
        .def("matrixSqrt", &SimulatorType::matrixSqrt)
        .def("getQubitRegister", &SimulatorType::getQubitRegister)        
*/

    //TinyMatrix
    py::class_<DCM>(m, "DCMatrix")
        .def(py::init<>())
        .def(py::init([](std::vector<std::complex<double>> &values) { 
            DCM m;
            m(0,0) = values[0];
            m(0,1) = values[1];
            m(1,0) = values[2];
            m(1,1) = values[3];
            return m;
        }))
        .def("__repr__",
            [](const DCM &s) {
                return s.tostr();
            }
        )
        .def("__getitem__", 
            [](const DCM &s, std::size_t i, std::size_t j) {
                if (i >= 2 || j >= 2) 
                    throw py::index_error();
                return s(i,j);
            })
        .def("__mul__", [](const DCM &s0, const DCM &s1) {
            DCM m;
            m(0,0) = s0(0,0)*s1(0,0) + s0(0,1)*s1(1,0);
            m(0,1) = s0(0,0)*s1(1,0) + s0(0,1)*s1(1,1);
            m(1,0) = s0(1,0)*s1(0,0) + s0(1,1)*s1(1,0);
            m(1,1) = s0(1,0)*s1(1,0) + s0(1,1)*s1(1,1);
            return m;
        })
        .def("__rmul__", [](const DCM &s0, std::complex<double> d1) {
            DCM m(s0);
            m(0,0) *= d1;
            m(0,1) *= d1;
            m(1,0) *= d1;
            m(1,1) *= d1;
            return m;
        })
        .def("__mul__", [](const DCM &s0, std::complex<double> d1) {
            DCM m(s0);
            m(0,0) *= d1;
            m(0,1) *= d1;
            m(1,0) *= d1;
            m(1,1) *= d1;
            return m;
        })
        .def("__radd__", [](const DCM &s0, std::complex<double> d1) {
            DCM m(s0);
            m(0,0) += d1;
            m(0,1) += d1;
            m(1,0) += d1;
            m(1,1) += d1;
            return m;
        })
        .def("__add__", [](const DCM &s0, const DCM &s1) {
            DCM m(s0);
            m(0,0) += s1(0,0);
            m(0,1) += s1(0,1);
            m(1,0) += s1(1,0);
            m(1,1) += s1(1,1);
            return m;
        })
        .def("__sub__", [](const DCM &s0, const DCM &s1) {
            DCM m(s0);
            m(0,0) -= s1(0,0);
            m(0,1) -= s1(0,1);
            m(1,0) -= s1(1,0);
            m(1,1) -= s1(1,1);
            return m;
        })
        .def("adjoint", [](const DCM &s0) {
            return SimulatorType::adjointMatrix(s0);;
        })
        .def("conjugate", [](const DCM &s0) {
            DCM m(s0);
            m(0,0) = std::conj(m(0,0));
            m(0,1) = std::conj(m(0,1));
            m(1,0) = std::conj(m(1,0));
            m(1,1) = std::conj(m(1,1));
            return m;
        })
        .def("transpose", [](const DCM &s0) {
            DCM m(s0);
            auto tmp = m(0,1);
            m(0,1) = m(1,0);
            m(1,0) = tmp;
            return m;
        })
        .def("as_numpy",[](DCM &s0){
            py::array_t<std::complex<double>> arr({ 2, 2 });
            py::buffer_info arr_buff = arr.request();
            std::complex<double>* ptr = (std::complex<double>*) arr_buff.ptr;

            ptr[0] = s0(0,0);
            ptr[1] = s0(0,1);
            ptr[2] = s0(1,0);
            ptr[3] = s0(1,1);

            return arr;
        });
}

PYBIND11_MODULE(_PyQNLPSimulator, m){
    intel_simulator_binding<IntelSimPy>(m);
}
