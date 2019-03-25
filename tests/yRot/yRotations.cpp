#include "util/tinymatrix.hpp"

#include "QubitCircuit.hpp"
#include <bitset>

/**
 * @brief Demonstrates relationship of state probabilities as qubit's are rotated
 * 
 */

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if(env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    unsigned n, num_exps;

    n = 2;
    num_exps = 10;

    double theta = M_PI/(double)num_exps;

    QubitCircuit<ComplexDP> circ(n,"base",0);

    circ.ApplyHadamard(0);
    circ.ApplyCPauliX(0,1);

    std::cout << "|0>  " << 0 << "\t" << circ.GetProbability(0) << "\t" << circ.GetProbability(1) << endl;
    std::cout << endl;

    for(int i = 0; i < num_exps; i++){
        circ.ApplyRotationY(0,theta);
        circ.ApplyCRotationY(0,1,0.1*theta);
        std::cout << "|0>  " << (i+1)*theta << "\t" << circ.GetProbability(0) << "\t" << circ.GetProbability(1) << endl;
    }

    return 0;

}

