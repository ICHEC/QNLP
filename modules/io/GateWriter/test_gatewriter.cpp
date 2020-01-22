//#define CATCH_CONFIG_RUNNER

#include "GateWriter.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

typedef ComplexDP Type;

template class NCU<IntelSimulator>;

/**
 * @brief Test n-controlled unitary module with Pauli-X |11..10> 
 * 
 */
TEST_CASE("Test n-controlled unitary module with Pauli-X |11..10>","[ncu]"){
    GateWriter writer;
    writer.oneQubitGateCall("label", )
}
