#include "Simulator.hpp"  
#include "IntelSimulator.cpp"

#include "db_helper.hpp"
#include "corpus_utils.hpp"

#include "CLI/CLI.hpp"

#include<bitset>

using namespace QNLP;

int main(int argc, char **argv){
    CLI::App app{"App description"};
    std::string filepath = "default";
    app.add_option("-f,--file", filepath, "The full path to the Database file");
    CLI11_PARSE(app, argc, argv);

    //Do not wish to interfere with the mpi environment for Intel QS
    char** empty;
    int empty_int = 0;
    openqu::mpi::Environment env(empty_int, empty);

    // Cannot understand why this exists, and is necessary... 
    // Commented as it prevents anything working
    //if (env.is_usefull_rank() == false) return 0;
    int rank = env.rank();

    /*
       Load data from pre-process corpus database
     */
    IntelSimulator sim(18);
    CorpusUtils cu(filepath);
    cu.loadData("noun");
    cu.loadData("verb");

    auto& ntb = cu.getNameToBin();

    //C++17 required for structured bindings
    for (const auto&[key, value] : ntb["noun"]) {
        std::cout << "Key = " << key << "   Value = " << value << std::endl;
    }
    for (const auto&[key, value] : ntb["verb"]) {
        std::cout << "Key = " << key << "   Value = " << value << std::endl;
    }
    std::cout << "File size: " << cu.getNameToBin().size() << std::endl;

    std::vector<std::size_t> reg_mem {0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<std::size_t> reg_anc {8, 9,10,11,12,13,14,15,16,17};
    std::vector<std::size_t> bin_patterns;

    std::size_t num_nouns = ntb["noun"].size();
    std::size_t num_verbs = ntb["verb"].size();
    std::cout << "Num_nouns = " << num_nouns << "   Num_verbs=" << num_verbs << std::endl;
    
    std::cout << "NOUNS" << std::endl;
    for (const auto&[key_ns, value_ns] : ntb["noun"]) {
        std::cout << key_ns << ":=" << value_ns << std::endl;
    }
    std::cout << "VERBS" << std::endl;
    for (const auto&[key_ns, value_ns] : ntb["verb"]) {
        std::cout << key_ns << ":=" << value_ns << std::endl;
    }

    std::vector<std::string> corpus_ns, corpus_no, corpus_v;

    corpus_ns.push_back("dogs");
    corpus_ns.push_back("cats");
    corpus_ns.push_back("jack");
    corpus_ns.push_back("chef");

    corpus_v.push_back("eat");
    corpus_v.push_back("hate");
    corpus_v.push_back("baked");
    corpus_v.push_back("walked");

    corpus_no.push_back("food");
    corpus_no.push_back("dogs");
    corpus_no.push_back("muffins");
    corpus_no.push_back("home");

    int num_samples=1;
    int result=-1;
    for(int sample=0; sample < num_samples; ++sample){
        bin_patterns.clear();
        sim.initRegister();

        for (const auto&[key_no, value_no] : ntb["noun"]) {
            for (const auto&[key_v, value_v] : ntb["verb"]) {
                for (const auto&[key_ns, value_ns] : ntb["noun"]) {
                    unsigned int pattern = 0;
/* 
                    std::cout << "NO=" << std::bitset<8>(value_no) << "     ";
                    std::cout << "V=" << std::bitset<8>(value_v) << "       ";
                    std::cout << "NS=" << std::bitset<8>(value_ns) << "     ";
                    */
                    pattern = value_no | (value_v << (int) ceil(log2(num_nouns))) | (value_ns << (int) (ceil(log2(num_verbs)) + ceil(log2(num_nouns)))); 
/*
                    std::cout << "PATTERN=" << std::bitset<8>(pattern) << "     ";
                    std::cout << "Shifts=" << (int) ceil( log2( num_nouns)) << "," << (int) (ceil(log2(num_verbs)) + ceil(log2(num_nouns))) << std::endl;
*/
                    bin_patterns.push_back(pattern);

                    //std::cout << "Keys[" << key_ns <<"," << key_v << "," << key_no << "]   VALUE = " << bin_patterns.back() << std::endl;
                    if(result == -1) //first time only                
                        std::cout << key_ns <<"," << key_v << "," << key_no << "=" << bin_patterns.back() << std::endl;
                }
            }
        }
        sim.encodeBinToSuperpos(reg_mem, reg_anc, bin_patterns, 8);
        result = sim.applyMeasurementToRegister(reg_mem);
        std::cout << result << std::endl;

        result = 0;
    }

    /*sim.encodeBinToSuperpos(reg_mem, reg_anc, bin_patterns, 6); 
    result = sim.applyMeasurementToRegister(reg_mem);
    std::cout << result << std::endl;
*/
    /*int num_samples=10000;
    int result = -1;
    for(int sample=0; sample < num_samples; ++sample){
        bin_patterns.clear();
        sim.initRegister();
        bin_patterns.push_back(2);
        bin_patterns.push_back(1);
        bin_patterns.push_back(1);
        //bin_patterns.push_back(4);
        //bin_patterns.push_back(5);
        //bin_patterns.push_back(3);
        sim.encodeBinToSuperpos(reg_mem, reg_anc, bin_patterns, 2); 
        result = sim.applyMeasurementToRegister(reg_mem);
        std::cout << result << std::endl;
    }*/
}
