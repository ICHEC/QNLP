#include "read_tags.hpp"

using namespace QNLP;

/**
 * @brief Simple test to load data from the DB, populate the maps, and print the data.
 * 
 * @param rt ReadTags object
 * @param dataType String representing the grammatical data type to load
 */
void testDataLoad(ReadTags& rt, std::string dataType){
    auto& b2n = rt.getBinToName();
    auto& n2b = rt.getNameToBin();
    std::cout << "#############################################################" << std::endl;
    std::cout << "Data size N2B:=" << rt.getNameToBin().size() << " B2N:=" << rt.getBinToName().size() << std::endl;
    std::cout << "#############################################################" << std::endl;

    std::cout << "Loading  '"<< dataType << "' data"<< std::endl;
    rt.loadData(dataType);
    std::cout << "#############################################################" << std::endl;

    rt.printData(dataType);
        std::cout << "#############################################################\n" << std::endl;
}

int main(){
    ReadTags rt {};

    assert(rt.getBinToName().size() == 0);
    assert(rt.getNameToBin().size() == 0);

    testDataLoad(rt, "noun");
    testDataLoad(rt, "verb");

    assert(rt.getBinToName().size() != 0);
    assert(rt.getNameToBin().size() != 0);

    return 0;
}