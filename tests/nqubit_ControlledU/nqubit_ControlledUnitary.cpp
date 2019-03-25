
//#define CHEAT
#include "util/tinymatrix.hpp"

#include "QubitCircuit.hpp"

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

#ifdef CHEAT
    int num_exps, m;
    std::size_t n;
    n = 8;                     // Number of qubit registers
    num_exps = 10;
    m = pow(2,n);       // Number of possible states

    int len_input = n-3;
    int len_ancilla = 2;
#else
    int len_input = 6;
    int len_ancilla = len_input-1;

    int num_exps, m;
    std::size_t n;
    n = len_ancilla + len_input + 1;                     // Number of qubit registers
    num_exps = 10;
    m = pow(2,n);       // Number of possible states
#endif


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

    QubitCircuit<ComplexDP> circ(n);

    openqu::TinyMatrix<ComplexDP, 2, 2, 32> U_not;
    U_not(0,0) = {0,0};
    U_not(0,1) = {1,0};
    U_not(1,0) = {1,0};
    U_not(1,1) = {0,0};

    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        circ.Initialize("base",0);

        for(int j = 0; j < len_input; j++){
            circ.ApplyPauliX(input[j]);
        }

        if(exp < len_input){
            circ.ApplyPauliX(input[exp]);
        }
         
#ifdef CHEAT
        //circ.ApplyNCPauliX( input,  ancilla,  target);
        circ.ApplyNCUnitary( input,  ancilla,  target, U_not);
#else
        //circ.ApplyNCPauliX( input,  ancilla,  target, len_input);
        circ.ApplyNCUnitary( input,  ancilla,  target, U_not, len_input);
#endif

        for(std::size_t j = 0; j < n; j++){
            circ.ApplyMeasurement(j);
        }

        // Obtain the probability of state being |1>
        vector<double> output(n);
        // Obtain the measured qubit 
        for(std::size_t j = 0; j < n; j++){
            //output[j] = circ.GetClassicalValue(j);
            output[j] = circ.GetProbability(j);
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

    return 0;
}
    
