#include "oracle.hpp"

#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;
template class Oracle<IntelSimulator>;

TEST_CASE("3 qubit oracles","[oracle]"){
    std::size_t num_qubits = 3;
    const std::vector<std::size_t> bit_patterns {0, 1, 2, 3};
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);
    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            sim.applyGateH(0);
            sim.applyGateH(1);
            sim.applyGateH(2);
            auto& r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], "");
            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());

            r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
            REQUIRE(r[i+4] != r[0]); //Not safest way to test for negation of the required bit
        }
    }
}

TEST_CASE("4 qubit oracles","[oracle]"){
    std::size_t num_qubits = 4;
    const std::vector<std::size_t> bit_patterns {0, 1, 2, 3, 4, 5, 6, 7};
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);
    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            sim.applyGateH(0);
            sim.applyGateH(1);
            sim.applyGateH(2);
            sim.applyGateH(3);

            auto& r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15], "");
            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());

            r = sim.getQubitRegister();
            CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15], "");

            REQUIRE(r[i + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
            for(std::size_t j = 0; j < bit_patterns.size(); j++){
                if(j==i)
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() < 0); //Not safest way to test for negation of the required bit
                else
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() > 0);
            }
        }
    }
}

TEST_CASE("8 qubit oracles","[oracle]"){
    std::size_t num_qubits = 8;
    std::vector<std::size_t> bit_patterns;
    for(std::size_t i = 0; i < pow(2,num_qubits-1); i++ ){
        bit_patterns.push_back(i);
    }
    IntelSimulator sim(num_qubits);
    std::vector<std::size_t> ctrl_indices;
    for(int i = 0; i < num_qubits-1; ++i){
        ctrl_indices.push_back(i);
    }
    REQUIRE(ctrl_indices.size() == num_qubits-1);
    REQUIRE(ctrl_indices[0] == 0);
    REQUIRE(ctrl_indices[1] == 1);
    REQUIRE(ctrl_indices[2] == 2);
    REQUIRE(ctrl_indices[3] == 3);
    REQUIRE(ctrl_indices[4] == 4);
    REQUIRE(ctrl_indices[5] == 5);
    REQUIRE(ctrl_indices[6] == 6);

    double mach_eps = (7./3.) - (4./3.) + (-1.);

    Oracle<decltype(sim)> oracle(num_qubits-1, ctrl_indices);

    for(auto &i : bit_patterns){
        DYNAMIC_SECTION("PauliZ with pattern " << i){
            sim.initRegister();
            for(int gate_idx = 0; gate_idx < num_qubits; gate_idx++){
                sim.applyGateH(gate_idx);
            }
            auto& r = sim.getQubitRegister();
            /*CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15],
                    r[16],r[17],r[18], r[19], r[20], r[21], r[22], r[23],
                    r[24],r[25],r[26], r[27], r[28], r[29], r[30], r[31],
                    r[33],r[34],r[35], r[36], r[37], r[38], r[39], r[40],
                    r[41],r[42],r[43], r[44], r[45], r[46], r[47], r[48],
                    r[49], r[50], r[51], r[52], r[53], r[54], r[55], 
                    r[56], r[57], r[58], r[59], r[60], r[61], r[62], r[63], 
                    r[64], r[65], r[66], r[67], r[68], r[69], r[70], r[71], 
                    r[72], r[73], r[74], r[75], r[76], r[77], r[78], r[79], 
                    r[80], r[81], r[82], r[83], r[84], r[85], r[86], r[87], 
                    r[88], r[89], r[90], r[91], r[92], r[93], r[94], r[95], 
                    r[96], r[97], r[98], r[99], r[100], r[101], r[102], r[103], 
                    r[104], r[105], r[106], r[107], r[108], r[109], r[110], r[111], 
                    r[112], r[113], r[114], r[115], r[116], r[117], r[118], r[119], 
                    r[120], r[121], r[122], r[123], r[124], r[125], r[126], r[127],
                    r[128], r[129], r[130], r[131], r[132], r[133], r[134], r[135], 
                    r[136], r[137], r[138], r[139], r[140], r[141], r[142], r[143], 
                    r[144], r[145], r[146], r[147], r[148], r[149], r[150], r[151], 
                    r[152], r[153], r[154], r[155], r[156], r[157], r[158], r[159], 
                    r[160], r[161], r[162], r[163], r[164], r[165], r[166], r[167], 
                    r[168], r[169], r[170], r[171], r[172], r[173], r[174], r[175], 
                    r[176], r[177], r[178], r[179], r[180], r[181], r[182], r[183], 
                    r[184], r[185], r[186], r[187], r[188], r[189], r[190], r[191], 
                    r[192], r[193], r[194], r[195], r[196], r[197], r[198], r[199], 
                    r[200], r[201], r[202], r[203], r[204], r[205], r[206], r[207], 
                    r[208], r[209], r[210], r[211], r[212], r[213], r[214], r[215], 
                    r[216], r[217], r[218], r[219], r[220], r[221], r[222], r[223], 
                    r[224], r[225], r[226], r[227], r[228], r[229], r[230], r[231], 
                    r[232], r[233], r[234], r[235], r[236], r[237], r[238], r[239], 
                    r[240], r[241], r[242], r[243], r[244], r[245], r[246], r[247], 
                    r[248], r[249], r[250], r[251], r[252], r[253], r[254], r[255], "");*/

            oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());
            
            /*CAPTURE(r[0], r[1], r[2],  r[3],  r[4],  r[5],  r[6],  r[7], 
                    r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15],
                    r[16],r[17],r[18], r[19], r[20], r[21], r[22], r[23],
                    r[24],r[25],r[26], r[27], r[28], r[29], r[30], r[31],
                    r[33],r[34],r[35], r[36], r[37], r[38], r[39], r[40],
                    r[41],r[42],r[43], r[44], r[45], r[46], r[47], r[48],
                    r[49], r[50], r[51], r[52], r[53], r[54], r[55], 
                    r[56], r[57], r[58], r[59], r[60], r[61], r[62], r[63], 
                    r[64], r[65], r[66], r[67], r[68], r[69], r[70], r[71], 
                    r[72], r[73], r[74], r[75], r[76], r[77], r[78], r[79], 
                    r[80], r[81], r[82], r[83], r[84], r[85], r[86], r[87], 
                    r[88], r[89], r[90], r[91], r[92], r[93], r[94], r[95], 
                    r[96], r[97], r[98], r[99], r[100], r[101], r[102], r[103], 
                    r[104], r[105], r[106], r[107], r[108], r[109], r[110], r[111], 
                    r[112], r[113], r[114], r[115], r[116], r[117], r[118], r[119], 
                    r[120], r[121], r[122], r[123], r[124], r[125], r[126], r[127],
                    r[128], r[129], r[130], r[131], r[132], r[133], r[134], r[135], 
                    r[136], r[137], r[138], r[139], r[140], r[141], r[142], r[143], 
                    r[144], r[145], r[146], r[147], r[148], r[149], r[150], r[151], 
                    r[152], r[153], r[154], r[155], r[156], r[157], r[158], r[159], 
                    r[160], r[161], r[162], r[163], r[164], r[165], r[166], r[167], 
                    r[168], r[169], r[170], r[171], r[172], r[173], r[174], r[175], 
                    r[176], r[177], r[178], r[179], r[180], r[181], r[182], r[183], 
                    r[184], r[185], r[186], r[187], r[188], r[189], r[190], r[191], 
                    r[192], r[193], r[194], r[195], r[196], r[197], r[198], r[199], 
                    r[200], r[201], r[202], r[203], r[204], r[205], r[206], r[207], 
                    r[208], r[209], r[210], r[211], r[212], r[213], r[214], r[215], 
                    r[216], r[217], r[218], r[219], r[220], r[221], r[222], r[223], 
                    r[224], r[225], r[226], r[227], r[228], r[229], r[230], r[231], 
                    r[232], r[233], r[234], r[235], r[236], r[237], r[238], r[239], 
                    r[240], r[241], r[242], r[243], r[244], r[245], r[246], r[247], 
                    r[248], r[249], r[250], r[251], r[252], r[253], r[254], r[255], ""); */

    /* #Small python script for generation of above formatted access to register
    num_qubits=4
    columns = 8            
    for i in range(0, 2**num_qubits, 1): 
        if(i % columns == 0): 
            print("") 
            print("r[{}]".format(i), end =", ")
    */

            for(std::size_t j = 0; j < bit_patterns.size(); j++){
                if(j==i)
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() <= 10*mach_eps  ) ; //Not safest way to test for negation of the required bit
                else
                    REQUIRE(r[j + (0b1<<(num_qubits-1))].real() + 10*mach_eps >= 0. );
            }
        }
    }
}
