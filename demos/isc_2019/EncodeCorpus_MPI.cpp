#include "Simulator.hpp"  
#include "IntelSimulator.cpp"

#include "db_helper.hpp"
#include "corpus_utils.hpp"

#include "CLI/CLI.hpp"

#include<bitset>
#include <omp.h>

using namespace QNLP;

int main(int argc, char **argv){
    CLI::App app{"App description"};
    std::string filepath = "default";
    app.add_option("-f,--file", filepath, "The full path to the Database file");
    CLI11_PARSE(app, argc, argv);

    unsigned int rank=0, num_procs=1;
#ifdef INTELQS_HAS_MPI
    //Do not wish to interfere with the mpi environment for Intel QS
    char** empty;
    int empty_int = 0;

    openqu::mpi::Environment env(empty_int, empty);

    rank = env.rank();
    num_procs = openqu::mpi::Environment::size();
    if (env.is_useful_rank() == false) return 0;
    MPI_Comm comm = openqu::mpi::Environment::comm();
    //std::cout << "MPI RANKS= " << rank  << std::endl;
#endif

    //std::cout << "OPENMP THREADS = " << omp_get_num_threads() << "CURRENT THREAD=" << omp_get_thread_num() << std::endl;
    /*
       Load data from pre-process corpus database
     */
    

    IntelSimulator sim(18, true);
    sim.getQubitRegister().TurnOnSpecialize();
    sim.getQubitRegister().EnableStatistics();

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

    std::vector<std::size_t> reg_mem {0, 1,  2, /**/ 3,  4, /**/  5,  6,  7};
    std::vector<std::size_t> reg_anc {8, 9, 10,/**/ 11, 12, /**/ 13, 14, 15, /**/ 16, 17};
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

    int num_samples=1;
    int result=-1;
    for(int sample=0; sample < num_samples; ++sample){
        bin_patterns.clear();
        sim.initRegister();

        unsigned int num_no = 0, num_v = 0, num_ns = 0;
        for (const auto&[key_no, value_no] : ntb["noun"]) {
            if (num_no >= 8-1){ continue; }
            num_no++;
            num_v = 0;
            for (const auto&[key_v, value_v] : ntb["verb"]) {
                if (num_v >= 4-1){ break; }
                num_v++;
                num_ns = 0;
                for (const auto&[key_ns, value_ns] : ntb["noun"]) {
                    if (num_ns >= 8-1){ break; }
                    num_ns++;
                    unsigned int pattern = 0;
                    //pattern = value_no | (value_v << (int) ceil(log2(num_nouns))) | (value_ns << (int) (ceil(log2(num_verbs)) + ceil(log2(num_nouns)))); 
                    pattern = num_ns | (num_v << 3 | (num_no << 5)); 
                    bin_patterns.push_back(pattern);

                    if(result == -1){         
                        std::cout << key_ns <<"," << key_v << "," << key_no << "=" << bin_patterns.back() << std::endl;
                    }
                }
            }
        }

        //std::cout << "OPENMP THREADS = " << omp_get_num_threads() << "CURRENT THREAD=" << omp_get_thread_num() << std::endl;
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
