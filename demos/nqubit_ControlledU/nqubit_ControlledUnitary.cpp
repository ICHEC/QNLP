#include "IntelSimulator.cpp"  
//#define CHEAT
#include "util/tinymatrix.hpp"

using namespace QNLP;

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned num_exps, m, n;
    n = 8;                     // Number of qubit registers
    num_exps = 10;
    m = pow(2,n);       // Number of possible states

    int len_input = n-3;
    int len_ancilla = 2;

    // Set registers' indices
    vector<std::size_t> input(len_input);
    vector<std::size_t> ancilla(len_ancilla);
    vector<std::size_t> target(1);
    for(std::size_t j = 0; j < len_input; j++){
        input[j] = j;
    }
    for(std::size_t j = 0; j < len_ancilla; j++){
        ancilla[j] = j + len_input;
    }
    target[0] = n-1;

    int count = 0;
    vector<double> output(n);

    SimulatorGeneral<IntelSimulator> *circ = new IntelSimulator(n);

    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        circ->initRegister();

        for(int j = 0; j < len_input; j++){
            circ->applyGateX(input[j]);
        }

        if(exp < len_input){
            circ->applyGateX(input[exp]);
        }
         
        circ->applyGateNCU(circ->getGateX(),ancilla[0],ancilla[len_ancilla-1], target[0]);

/*
        for(std::size_t j = 0; j < n; j++){
            circ->ApplyMeasurement(j);
        }
 
        // Obtain the probability of state being |1>
        // Obtain the measured qubit 
        for(std::size_t j = 0; j < n; j++){
            //output[j] = circ->GetClassicalValue(j);
            output[j] = circ->GetProbability(j);
        }



        // Increase the count if the state is |1111>|00>|1>
        int tmp_count = 0;
        for(int j = 0; j < len_input; j++){
            if(1 != output[j]){
                break;
            }
            tmp_count++;
        }
        for(int j = len_input; j < n-1; j++){
            if(0 != output[j]){
                break;
            }
            tmp_count++;
        }
        if(output[n-1] == 1){
            tmp_count++;
        }
        if(tmp_count == n){
            count++;
        }

        cout << "|";
        for(int j = 0; j < len_input; j++){
            cout << output[input[j]];
        }
        cout << ">";

        cout << "|";
        for(int j = 0; j < len_ancilla; j++){
            cout << output[ancilla[j]];
        }
        cout << ">";
        cout << "|";
        cout << output[target[0]];
        cout << ">" << endl;



    
    }
            
    cout << "Measure:" << endl;
    cout << '\t' << count << '\t' << 100.0*(double)count/(double) num_exps << "%" <<  endl;
*/
    }

    cout << "Fin" << endl;
    return 0;
}
    
