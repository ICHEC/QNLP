/**
 * @file sim_factory.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Implementation of Factory methods for generating the chosen subclassed Simulator object
 * @version 0.1
 * @date 2019-02-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "../include/sim_interface.hpp"
#include "../include/qhipster_interface.hpp"

//#include <stdexcept>
#include <memory>

using namespace QNLP;

//Add new backends to the enum here.
enum SimBackend { qhipster=0, unknown=1 };

/**
 * @brief Create a Simulator object
 * 
 * @param s index of simulator backend in the above enum
 * @param numQubits number of qubits to create in simulator register.
 * @return std::unique_ptr<Simulator> returns pointer to the chosen subclassed simulator
 */
std::unique_ptr<SimulatorInterface> SimulatorInterface::createSimulator(SimBackend s, std::size_t numQubits){
    //For new backends, please extend the switch case below.
    switch( s ){
        case SimBackend::qhipster: 
            return std::make_unique<IntelSimulator>(numQubits);
        default:
            printf("No simulator chosen. Exitting.");
            exit(-1);//throw std::runtime_error("Unknown simulator backend.");
    }
}

