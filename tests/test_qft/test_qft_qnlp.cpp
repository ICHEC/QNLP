#include "qft.hpp"

#include "qureg/qureg.hpp"
#include "util/tinymatrix.hpp"
#include <bitset>

using namespace QNLP;

void test_rotate_iqft_full(std::size_t num_qubits, bool include_rot_qubit){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    double dtheta = 2.*M_PI/(1<<num_qubits);
    double p = -1.;
    int shifts = 0;

    //#########################################################################

    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        for (double theta = 0.; theta < M_PI*2.; theta += dtheta){
            psi1.Initialize("base",0);

            //Define the given bit-pattern
            for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
                unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
                if(window_select == 1){
                    psi1.ApplyPauliX(ctrl_qubit);
                }
            }

            //Test arbitrary rotation now
            psi1.ApplyRotationY(num_qubits-1, theta);

            //Examine last qubit
            unsigned int offset = (include_rot_qubit == true) ? 0 : 1;
            Ops::applyIQFT(psi1, 0, num_qubits - offset);
            //std::cout << " Prob(" << cc << ", " << theta <<", " << include_rot_qubit <<")=" << psi1.GetProbability(num_qubits-1) << std::endl;
            std::cout << cc << "," << theta << "," << include_rot_qubit << "," << psi1.GetProbability(num_qubits-1) << std::endl;
        }
    }
}

void test_rotate_iqft(std::size_t num_qubits, unsigned int pattern, bool include_rot_qubit){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);
    double angle = M_PI/(1<<num_qubits);
    double current_angle = 0.;
    double p = -1.;
    int shifts = 0;
    for(unsigned int ii = 1; ii <= (1 << num_qubits); ii*=2){
        if((pattern & ii) > 0){
            psi1.ApplyPauliX(shifts);
        }
        current_angle += angle;
        shifts++;
    }

    for(int i = 0; i < num_qubits-1; i++){
        psi1.ApplyCRotationY(i, num_qubits-1, angle);
    } 

    unsigned int offset = (include_rot_qubit == true) ? 0 : 1;
    Ops::applyIQFT(psi1, 0, num_qubits - offset);
    std::cout << " Th(" << current_angle <<", " << include_rot_qubit <<")"; 
    std::cout << " prob=" << psi1.GetProbability(num_qubits-1) << std::endl;
}

void test_qft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);

    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);

        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }
        Ops::applyQFT(psi1, 0, num_qubits);
        std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

void test_iqft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);

    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);

        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }

        Ops::applyIQFT(psi1, 0, num_qubits);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

void test_qft_iqft(std::size_t num_qubits){
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);

    double p = -1.;
    for( unsigned int cc = 0; cc < 1<<(num_qubits-1); cc++ ){
        psi1.Initialize("base",0);

        for (int ctrl_qubit = 0; ctrl_qubit < num_qubits-1; ctrl_qubit ++){
            unsigned int window_select = (cc>>ctrl_qubit) & 0b1;
            if(window_select == 1){
                psi1.ApplyPauliX(ctrl_qubit);
            }
        }

        Ops::applyQFT(psi1, 0, num_qubits);
        Ops::applyIQFT(psi1, 0, num_qubits);
        std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

int main(int argc, char **argv){

    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    {
        if (rank == 0) {
            printf("\n --- n-Qubit QFT --- \n");
        }
        unsigned int num_qubits = 5;
        std::cout <<"#" << "bit-pattern" << "," << "theta" << "," << "include_rotation_bit" << "," << "P(1)" << std::endl;
        test_rotate_iqft_full(num_qubits, true);
        test_rotate_iqft_full(num_qubits, false);
        return 0;

    }
