/**
 * @file demo_nqubit_ControlledUnitary.cpp
 * @author Myles Doyle (myles.doyle@ichec.ie)
 * @brief Apply NCU with input data as control on a target qubit in state 0. The U matrix will be PailiX for simplicity. Outputs the final measured state. Displays proportion of experiments with target qubit set.
 * @version 0.1
 * @date 2020-01-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "IntelSimulator.cpp"  
#include "Simulator.hpp"  

using namespace QNLP;

int main(int argc, char **argv){

    unsigned num_exps, m, n;
    n = 8;                     // Number of qubit registers
    num_exps = 10;
    m = pow(2,n);       // Number of possible states
    int len_input = n-1;

    SimulatorGeneral<IntelSimulator> *sim = new IntelSimulator(n);

    int rank;
#if ENABLE_MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
    rank = 0;
#endif

    // Set registers' indices
    vector<std::size_t> input(len_input);
    vector<std::size_t> target(1);
    for(std::size_t j = 0; j < len_input; j++){
        input[j] = j;
    }
    target[0] = n-1;

    int count = 0;
    vector<double> output(n);


    cout << "|input>|target>" << endl;

    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        sim->initRegister();

        // Set input to register to |11...1>
        for(int j = 0; j < len_input; j++){
            sim->applyGateX(input[j]);
        }

        if(exp < len_input){
            sim->applyGateX(input[exp]);
        }
         
        sim->applyGateNCU(sim->getGateX(), input, target[0], "X");

        for(std::size_t j = 0; j < n; j++){
            output[j] = sim->applyMeasurement(j);
        }

        // Increase the count if the target register is |1>
        if(output[n-1]){
            count++;
        }

        if(rank == 0){
            // Output resulting state for this experiment
            cout << "|";
            for(int j = 0; j < len_input; j++){
                cout << output[input[j]];
            }
            cout << ">";
            cout << "|";
            cout << output[target[0]];
            cout << ">" << endl;
        }
    }
            
    if(rank == 0){
        cout << "Measure:" << endl;
        cout << '\t' << count << '\t' << 100.0*(double)count/(double) num_exps << "%" <<  endl;
    }

    return 0;
}
    
