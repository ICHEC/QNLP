/**
 * @file test_qft.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Tests for the quantum Fourier transform (QFT) implementation. 
 * @version 0.1
 * @date 2019-05-13
 * 
 * @copyright Copyright (c) 2019
 * 
 */
//#define CATCH_CONFIG_RUNNER
//#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include <memory>
#include "qft.hpp"

using namespace QNLP;
#include <bitset>

TEST_CASE("QFT forward","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);
    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        //template class QFT<decltype(sim)>;
        //QFT<decltype(sim)>::applyQFT(sim, min_idx, max_idx);
        sim.applyQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / num_qubits ) ) );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx+1, max_idx);
        REQUIRE( r.GetProbability(min_idx) == 0.0 );
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / (num_qubits-1) ) ) );
        }
    }
}

TEST_CASE("QFT inverse","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);
    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyIQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / num_qubits ) ) );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyIQFT(min_idx+1, max_idx);
        REQUIRE( r.GetProbability(min_idx) == 0.0 );
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == Approx( sqrt( 1. / (num_qubits-1) ) ) );
        }
    }
}
TEST_CASE("QFT forward and inverse","[qft]"){
    std::size_t min_idx=0, max_idx=3;
    std::size_t num_qubits = max_idx - min_idx +1;
    IntelSimulator sim(num_qubits);

    SECTION("Full register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx, max_idx);
        sim.applyIQFT(min_idx, max_idx);
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
    }
    SECTION("Sub register"){
        auto& r = sim.getQubitRegister();
        for(std::size_t idx = min_idx; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
        sim.applyQFT(min_idx+1, max_idx);
        sim.applyIQFT(min_idx+1, max_idx);
        for(std::size_t idx = min_idx+1; idx <= max_idx; idx++){
            REQUIRE( r.GetProbability(idx) == 0.0 );
        }
    }
}
/*
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
*/

//#############################################################################
/*
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
            unsigned int offset = (include_rot_qubit == true) ? 1 : 2;
            Util::applyIQFT(psi1, 0, num_qubits - offset);
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

    unsigned int offset = (include_rot_qubit == true) ? 1 : 2;
    Util::applyIQFT(psi1, 0, num_qubits - offset);
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
        Util::applyQFT(psi1, 0, num_qubits);
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

        Util::applyIQFT(psi1, 0, num_qubits);
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

        Util::applyQFT(psi1, 0, num_qubits-1);
        Util::InvertRegister(psi1, 0, num_qubits-1);
        Util::applyIQFT(psi1, 0, num_qubits-1);
        Util::InvertRegister(psi1, 0, num_qubits-1);
        std::cout << "Test " << cc << "\t\tPattern=" << std::bitset<8>(cc);
        std::cout << "\t\tP(0,1)=["; 
        for(int qb = 0; qb < num_qubits; qb++){
            std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
        }
        std::cout << "]" << std::endl;
    }
}

void test_swap(unsigned int num_qubits){
    assert(num_qubits%2 == 0);
    QubitRegister<ComplexDP> psi1(num_qubits, "base", 0);    
    for(int i =0; i < num_qubits; i+=2){
        psi1.ApplyPauliX(i);
    }
    for(int qb = 0; qb < num_qubits; qb++){
        std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
    }
    std::cout << "\n";
    Util::InvertRegister(psi1, 0, num_qubits-1);
    for(int qb = 0; qb < num_qubits; qb++){
        std::cout << "[" << 1. - psi1.GetProbability( qb ) << "," << psi1.GetProbability( qb ) << "],";
    }
    std::cout << "\n";
}

int main(int argc, char **argv){
    openqu::mpi::Environment env(argc, argv);
    if (env.is_usefull_rank() == false) return 0;

    int rank = env.rank();

    {
        if (rank == 0) {
            printf("\n --- n-Qubit QFT --- \n");
        }
        unsigned int num_qubits = 4;
        std::cout <<"#" << "bit-pattern" << "," << "theta" << "," << "include_rotation_bit" << "," << "P(1)" << std::endl;

        //test_rotate_iqft_full(num_qubits, true);
        //test_rotate_iqft_full(num_qubits, false);
        //test_qft_iqft(num_qubits);
        test_swap(num_qubits);
        return 0;

    }
}

*/
