#include "Simulator.hpp"  
#include "IntelSimulator.cpp"

#include "db_helper.hpp"
#include "corpus_utils.hpp"

#include "CLI/CLI.hpp"

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
    IntelSimulator sim(8);
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

    std::vector<std::size_t> reg_mem;
    std::vector<std::size_t> reg_anc;
    std::vector<std::size_t> bin_patterns;
    
    //sim.encodeBinToSuperpos(std::vector<std::size_t>{0,1,2,3}, );
}