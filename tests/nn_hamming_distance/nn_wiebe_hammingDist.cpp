#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"

#include "QubitCircuit.hpp"

// Set vectors containing appropriate indices of each qubit in each register in the overall circuit
void set_circ_register_indexing(vector<int>& x,vector<int>& g,vector<int>& c, vector<int>& input, int m, int n){

    int i = 0, j = 0;
    for(i = 0; i < n; i++){
        x[i] = i;
    }
    g[0] = n;
    c[0] = n+1;
    c[1] = n+2;
    for(i = n+3; i < 2*n+3; i++){
        input[j] = i;
        j++;
    }
    
}


// Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
//              - Let there exist m vectors of binary number, each of length n.
//              - Require 2n+1 qubits
template <class Type>
void encode_binarystrings(vector<vector<int>> pattern, QubitCircuit<Type>& circ, int m, int n){

        // We wil then prepare three matrices S^1, S^2 and S^3 that are required for the implemented
        // algorithm to encode these binary strings into a superposition state.
        //
        // Note the matrix indexing of the S vector of S^p matrices will be backwards:
        //      S[0] -> S^p
        //      S[1] -> S_{p-1}, and so on.
        //
        vector<openqu::TinyMatrix<Type, 2, 2, 32>> S(m);


        // Step 1.1     - Generate three registers, x, g and c.
        //                      - x holds superposition of patterns. We require 1 qubit in this register for 
        //                      each bit in the pattern to be stored
        //                      - g is a garbage register only used to identify a particular state
        //                      - c is the control register consisting of two qubits whci indicate the status
        //                      of each state as the superposition is being created
        //
        // Use vectors to store indices of appropriate registers in circuit
        vector<int> x(n);
        vector<int> g(1);
        vector<int> c(2);
        vector<int> dummy(2);

        set_circ_register_indexing( x, g, c, dummy,  m,  n);

        // Define unitary matrices S^p
        {
            int p = m;
            double diag, off_diag;

            for(int i = 0; i < m; i++){
                diag = sqrt((double)(p-1)/(double)p);
                off_diag = 1.0/sqrt((double)p);

                S[i](0,0) = {diag,         0.0};
                S[i](0,1) = {off_diag,     0.0};
                S[i](1,0) = {-1.0*off_diag,0.0};
                S[i](1,1) = {diag,         0.0};
                p--;
            }
        }         

/*
        for(int i = 0; i < m; i++){
            cout << " S^" << m-i << " matrix" << endl;
            cout << S[i](0,0) << "\t" << S[i](0,1) << endl;
            cout << S[i](1,0) << "\t" << S[i](1,1) << endl;
            cout << endl;
        }
*/

        // Initialise c register to state |01>
        circ.ApplyPauliX(c[1]);


        // Execute first iteration for first pattern

        // Step 1.2 - Encode pattern into quantum register x 
        // controlled if second control bit is one.
        for(int j = 0; j < n; j++){
            if(pattern[0][j] == 1){
                circ.ApplyCPauliX(c[1],x[j]);
            }
        }
        // Flip first control qubit if second is |1>
        circ.ApplyCPauliX(c[1],c[0]);

        // Set extra garbage qubit g to |1> if c is |11>
        circ.ApplyToffoli(c[0],c[1],g[0]);

        // Step 1.3 - Apply S^i
        // Apply S^{n-j} = S[j] to the control qubits if c is |11>
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        circ.ApplyControlled1QubitGate(g[0],c[1],S[0]);

        // Step 1.4 - SAVE - flip
        //  
        // Superposition now stored in x.
        // Need to reset c and x registers for the newly generated term in superposition
        //
        // Flip all qubits in state |1> in the register g 
        circ.ApplyCPauliX(c[0],g[0]);
        // Set c from |11> -> |01> and |10> -> |00> (also |00> -> |10>)
        circ.ApplyPauliX(c[0]);
      


        // Encode each pattern into the superposition
        for(int i = 1; i < m; i++){
    
            // Step 1.2 - Encode pattern into quantum register x 
            // controlled if second control bit is one.
            for(int j = 0; j < n; j++){
                if(pattern[i][j] == 1 && pattern[i-1][j] != 1){
                    circ.ApplyCPauliX(c[1],x[j]);
                }
            }
            // Flip first control qubit if second is |1>
            circ.ApplyCPauliX(c[1],c[0]);

            // Set extra garbage qubit g to |1> if c is |11>
            circ.ApplyToffoli(c[0],c[1],g[0]);

            // Step 1.3 - Apply S^i
            // Apply S^{n-j} = S[j] to the control qubits if c is |11>
            // This flips the second control bit of the new term in the position so
            // that we get old|11> + new|10>
            circ.ApplyControlled1QubitGate(g[0],c[1],S[i]);

            // Step 1.4 - SAVE - flip
            //  
            // Superposition now stored in x.
            // Need to reset c and x registers for the newly generated term in superposition
            //
            // Flip all qubits in state |1> in the register g 
            circ.ApplyCPauliX(c[1],g[0]);       // ?? c[1] or c[0]
            // Set c to |01>
            circ.ApplyPauliX(c[0]);

        }       
}


template <class Type>
void compute_HammingDistance(vector<vector<int>> pattern, vector<vector<int>> input_pattern, QubitCircuit<Type>& circ, int m, int n){
    // Use vectors to store indices of appropriate registers in circuit
    vector<int> x(n);
    vector<int> g(1);
    vector<int> c(2);
    vector<int> input(n);

    set_circ_register_indexing( x, g, c, input,  m,  n);


    // Step 2       - Hamming distance calculation
    //
    // Step 2.1      - Encode test pattern into quantum register input
    for(int j = 0; j < n; j++){
        if(input_pattern[0][j] == 1){
            circ.ApplyCPauliX(c[1],input[j]);
        }
    }
    // Apply Hadamard to the Ancilla bit g
    circ.ApplyHadamard(g[0]);
    
    // Step 2.2     
    //              - Apply CX with control of the superpositions and target the input vector
    //              - Apply X to all input values
    for(int j = 0; j < n; j++){
        circ.ApplyCPauliX(x[j],input[j]);
        circ.ApplyPauliX(input[j]);
    }

    // Step 2.3     - Compute Hamming distance with the distances recorded in the input register
    // Define unitary matrices U[i] for each term in superposition
    vector<openqu::TinyMatrix<Type, 2, 2, 32>> U(3);
    {
        double cos_term, sin_term, pi_term;

        // U for H=Idnetity
        pi_term = 0.5*M_PI/(double)n;
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[0](0,0) = {cos_term,     -sin_term};
        U[0](0,1) = {0.0,          0.0};
        U[0](1,0) = {0.0,          0.0};
        U[0](1,1) = {cos_term,     -sin_term};

        // U for H=PauliZ
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[1](0,0) = {cos_term,     -sin_term};
        U[1](0,1) = {0.0,          0.0};
        U[1](1,0) = {0.0,          0.0};
        U[1](1,1) = {cos_term,     sin_term};

        // U for H=(PauliZ +1)/2
        pi_term *= 0.5;
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[2](0,0) = {cos_term*cos_term - sin_term*sin_term,     -2.0*cos_term*sin_term};
        U[2](0,1) = {0.0,          0.0};
        U[2](1,0) = {0.0,          0.0};
        U[2](1,1) = {1.0,          0.0};
    }         

    // Apply unitary with H=Identiy to the training data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(x[j],U[0]);
    }

    // Apply unitary with H=PauliZ to the ancillary register
    circ.Apply1QubitGate(g[0],U[1]);

    // Apply unitary with H=(PauliZ+1)/2 to the input data registers
    for(int j = 0; j < n; j++){
        circ.Apply1QubitGate(input[j],U[2]);
    }


    // Step 2.4     - Apply Hadamard to ancilla bit again
    circ.ApplyHadamard(g[0]);


}

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    int m, n;
    int num_exps;
    m = 3;
    n = 4;
    num_exps = 1000;

    int N = 2*n + 1;

    std::size_t tmpSize = 0;


    // To store initial patterns
    vector<vector<int>> pattern(m,vector<int>(n,0));
    vector<vector<int>> input_pattern(m,vector<int>(n,0));
    vector<int> count(m);

    // Initialise binary patterns
    pattern[0] = {0,0,0,1};
    pattern[1] = {0,1,1,1};
    pattern[2] = {1,1,1,1};
//    pattern[3] = {1,1,0,1};

    input_pattern[0] = {0,0,0,1};

    // Use vectors to store indices of appropriate registers in circuit
    //      - x holds superposition of training data
    //      - g is garbage register - also named ancilla qubit
    //      - c is an ancillary control register
    //      - input is used to store the input data
    vector<int> x(n);
    vector<int> g(1);
    vector<int> c(2);
    vector<int> input(n);

    set_circ_register_indexing( x, g, c,  input,  m,  n);


    //QubitCircuit<ComplexDP> circ(2*n+2+1, "base", 0);
    QubitCircuit<ComplexDP> circ((std::size_t)2*n+2+1);

    // Repeated shots of experiment
    for(int exp = 0; exp < num_exps; exp++){

        circ.Initialize("base",0);
         
        // Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
        //              - Let there exist m vectors of binary number, each of length n.
        //
        // Encode input binary patterns into superposition in registers for x.
        //      Note: after this operation, have (1/m)*|pattern_1>|A,0>|0> where A might be
        //      either a 0 or a one, for each input pattern.
        encode_binarystrings<ComplexDP>( pattern, circ, m, n);


        // Step 2      - Compute the Hamming distance between the input pattern and each training pattern.
        //             - Results are stored in the coefficient of each state of the input pattern
//        compute_HammingDistance<ComplexDP>( pattern, input_pattern, circ,  m,  n);

        // Apply measurement
        for(std::size_t j = 0; j < n; j++){
            circ.ApplyMeasurement(input[j]);
        }

 //       circ.ApplyMeasurement(g[0]);

        // Obtain the probability of state being |1>
        vector<double> output(n);
        // Obtain the measured qubit 
        for(std::size_t j = 0; j < n; j++){
            output[j] = circ.GetProbability(input[j]);
        }

        // Increase the count of the pattern being measured
        int tmp_count;
        for(int i = 0; i < m; i++){
            // Measure amplitude of the states with a cosine term /9ie with register g in |0>)
 //           if(circ.GetProbability(g[0]) == 0){
                tmp_count = 0;
                for(int j = 0; j < n; j++){
                    if(pattern[i][j] != output[j]){
                        break;
                    }
                    tmp_count++;
                }
                if(tmp_count == n){
                    count[i]++;
                }
   //         }
        }

    }



    cout << "Measure:" << endl;
    double perc;
    for(int i = 0; i < m; i++){
        perc = (double)count[i]/(double) num_exps;
        cout << i << '\t' << count[i] << '\t' << perc << "%\t" << ((double)2*n)*acos(perc)/M_PI <<  endl;
    }

}
