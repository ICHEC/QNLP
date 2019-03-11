#include "util/tinymatrix.hpp"

#include "QubitCircuit.hpp"

// Set vectors containing appropriate indices of each qubit in each register in the overall circuit
//      x       - n
//      input   - n
//      c       - m + 1
//      g       - 1
void set_circ_register_indexing(vector<int>& x, vector<int>& input, vector<int>& c, vector<int>& g, int m, int n){
    for(int i = 0; i < n; i++){
        x[i] = i;
    }
    for(int i = 0; i < n; i++){
        input[i] = i + n;
    }
    for(int i = 0; i < m + 1; i++){
        c[i] = i + 2*n;
    }
    g[0] = m + 2*n + 1;
}

// Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
//              - Let there exist m vectors of binary number, each of length n.
//              - Require m+2n+2 qubits
template < class Type >
void encode_binarystrings(vector<vector<int>> pattern, QubitCircuit<Type>& circ, int m, int n){
 
     // Use vectors to store indices of appropriate registers in circuit 
     //      - x holds superposition of training data
     //      - c is an ancillary control register
     //      - g is garbage register - also named ancilla qubit
     vector<int> x(n);
     vector<int> dummy(n);
     vector<int> c(m+1);
     vector<int> g(1);

    set_circ_register_indexing(x,dummy,c,g,m,n);

    // We wil then prepare three matrices S^1,S^2,...,S^3 that are required for the implemented
    // algorithm to encode these binary strings into a superposition state.
    //
    // Note the matrix indexing of the S vector of S^p matrices will be backwards:
    //      S[0] -> S^p
    //      S[1] -> S_{p-1}, and so on.
    //
    vector<openqu::TinyMatrix<Type, 2, 2, 32>> S(m);
    {
        int p = m;
        double diag, off_diag;

        for(int i = 0; i < m; i++){
            off_diag = 1.0/sqrt((double)(p));
            diag = off_diag * sqrt((double)(p-1));

            S[i](0,0) = {diag, 0.0};
            S[i](0,1) = {off_diag, 0.0};
            S[i](1,0) = {-off_diag, 0.0};
            S[i](1,1) = {diag, 0.0};

            p--;
        }
    }

//    
//    for(int i = 0; i < m; i++){
//        cout << " S^" << m-i << " matrix" << endl;  
//        cout << S[i](0,0) << "\t" << S[i](0,1) << endl;  
//        cout << S[i](1,0) << "\t" << S[i](1,1) << endl; 
//        cout << endl; 
//    }

    // Prepare state in |...>|10...>|0>
    circ.ApplyPauliX(c[0]);

    circ.ApplyCPauliX(c[0],c[1]);

    // Encode inputted binary pattern
    for(int j = 0; j < n; j++){
        if(pattern[0][j] == 1){
            circ.ApplyCPauliX(c[1],x[j]);
        }
    }

    // Step 1.3 - Apply S^i
    // This flips the second control bit of the new term in the position so
    // that we get old|11> + new|10>
    // Break off into larger and smaller chunks
    circ.ApplyControlled1QubitGate(c[0], c[1], S[0]);
     
    for(int i = 1; i < m; i++){
        circ.ApplyCPauliX(c[i],c[i+1]);

        // Encode inputted binary pattern
        for(int j = 0; j < n; j++){

            if(pattern[i-1][j] == 1){
                circ.ApplyCPauliX(c[i+1],x[j]);
            }
            if(pattern[i][j] == 1){
                circ.ApplyCPauliX(c[i+1],x[j]);
            }
        }

        // Step 1.3 - Apply S^i
        // This flips the second control bit of the new term in the position so
        // that we get old|11> + new|10>
        // Break off into larger and smaller chunks
        circ.ApplyControlled1QubitGate(c[i], c[i+1], S[i]);
    }
        
   
}

 
template <class Type>
void compute_HammingDistance(vector<vector<int>> pattern, vector<vector<int>> input_pattern, QubitCircuit<Type>& circ, int m, int n);
 

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned m, n, total_qubits, num_exps;
    
    m = 4;
    n = 2;
    num_exps = 10000;

    total_qubits = m + 2*n + 1 + 1;

    // To store initial patterns 
    vector<vector<int>> pattern(m, vector<int> (n,0));
    vector<vector<int>> input_pattern(1, vector<int> (n,0));

      pattern[0] = {0,1};
      pattern[1] = {1,0};
      pattern[2] = {0,0};
      pattern[3] = {1,1};
      input_pattern[0] = {1,1};

//
//      pattern[0] = {1,1,0,0};
//      pattern[1] = {0,0,1,1};
//      pattern[2] = {0,0,0,0};
//      pattern[3] = {1,1,1,1};
//      pattern[4] = {1,0,1,1};
//
//      input_pattern[0] = {0,0,1,1};

     // Use vectors to store indices of appropriate registers in circuit 
     //      - x holds superposition of training data
     //      - c is an ancillary control register
     //      - g is garbage register - also named ancilla qubit
     vector<int> x(n);
     vector<int> input(n);
     vector<int> c(m+1);
     vector<int> g(1);

    set_circ_register_indexing(x,input,c,g,m,n);

    // Declare quantum circuit
    QubitCircuit<ComplexDP> circ(total_qubits);


    vector<double> output(n);
    vector<double> count(m);


    double p1, p2;

    for(int exp = 0; exp < num_exps; exp++){
        circ.Initialize("base",0);

        // Step 1       - Encode states into a superposition. See Ventura, 2000, Quantum associative memory            
        //              - Let there exist m vectors of binary number, each of length n.
        //
        // Encode input binary patterns into superposition in registers for x.
        encode_binarystrings<ComplexDP>(pattern, circ, m, n);

        // Step 2      - Compute the Hamming distance between the input pattern and each training pattern.
        //             - Results are stored in the coefficient of each state of the input pattern
//        compute_HammingDistance<ComplexDP>( pattern, input_pattern, circ,  m,  n);



        for(int j = 0; j < n; j++){
            circ.ApplyMeasurement(x[j]);
        }

        circ.CollapseQubit(g[0],0);
        circ.Normalize();

        for(int j = 0; j < n; j++){
            output[j] = circ.GetProbability(x[j]);
        //    cout << output[j] << "\t";
        }
       // cout << endl;


        // Increase the count of the pattern being measured
        int tmp_count;
        for(int i = 0; i < m; i++){
            // Measure amplitude of the states with a cosine term /9ie with register g in |0>)
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
        }
    }

    for(int i = 0; i < m; i++){
        cout << "|";
        for(int j = 0; j < n; j++){
            cout << pattern[i][j];
        }
        cout << ">\t" << count[i] << "\t" << count[i]/(double)num_exps * 100.0 << "%" << endl;
    }


    cout << "TestPattern = " << "|";
    for(int i = 0; i < n; i++){
        cout << input_pattern[0][i];
    }
    cout << ">" << endl;;


    cout << "state\tfreq\tprob\tdist\tProb(D)" << endl;
    double costerm, dist;
    double prob, prob_sum;
    prob_sum = 0.0;
    for(int i = 0; i < m; i++){
        prob = (double)count[i]/(double) num_exps;

        prob_sum += prob;


        dist = ((double)2*n)*acos(prob*sqrt((double)m))/M_PI;
        cout << i << '\t' << count[i] << '\t' << prob*100.0 << "%\t" << dist  << "\t"<< dist/(double)m<<  endl;
    }
    cout << "Sum of probs =\t" << prob_sum << endl;

    return 0;

}

        
template <class Type>
void compute_HammingDistance(vector<vector<int>> pattern, vector<vector<int>> input_pattern, QubitCircuit<Type>& circ, int m, int n){
    // Use vectors to store indices of appropriate registers in circuit
    vector<int> x(n);
    vector<int> input(n);
    vector<int> c(m+1);
    vector<int> g(1);

    set_circ_register_indexing( x, input, g, c, m,  n);

    // Step 2       - Hamming distance calculation
    //
    // Step 2.1      - Encode test pattern into quantum register input
    for(int j = 0; j < n; j++){
        if(input_pattern[0][j] == 1){
            circ.ApplyPauliX(input[j]);
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
        U[1](0,0) = {cos_term,     -sin_term};
        U[1](0,1) = {0.0,          0.0};
        U[1](1,0) = {0.0,          0.0};
        U[1](1,1) = {cos_term,     sin_term};

        // U for H=(PauliZ +1)/2
        pi_term *= 0.5;
        cos_term = cos(pi_term);
        sin_term = sin(pi_term);
        U[2](0,0) = {2.0*cos_term*cos_term - 1.0,     -2.0*cos_term*sin_term};
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

//  ???
    // Apply unitary with H=Identiy to the training data registers
//    for(int j = 0; j < m+1; j++){
  //      circ.Apply1QubitGate(c[j],U[0]);
    //}




    // Step 2.4     - Apply Hadamard to ancilla bit again
    circ.ApplyHadamard(g[0]);

}
