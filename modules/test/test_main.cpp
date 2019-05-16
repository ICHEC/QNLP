//#define CATCH_CONFIG_NOSTDOUT
#define CATCH_CONFIG_RUNNER

#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"
#include <fstream> 
#include <iostream> 

/**
 * Following Catch2/examples/231-Cfg-OutputStreams.cpp example to define 
 * custom output streams. Required to avoid pollution of reporting from
 * cout and cerr in the generated binaries.
 */

/*
class out_buff : public std::stringbuf {
    std::FILE* m_stream;
    std::ofstream outfile;
public:
    out_buff(std::FILE* stream, std::string fileName) :m_stream(stream), outfile(fileName) {
    }
    ~out_buff() { 
        while ((c = std::fgetc(fp)) != EOF) { // standard C I/O file reading loop
                       std::putchar(c);
                           }
        outfile << str();
        pubsync();
        outfile.close();
    }
};

namespace Catch {
    std::ostream& cout() {
        static std::ostream ret(new out_buff(stdout, "test1"));
        return ret;
    }
    std::ostream& clog() {
        static std::ostream ret(new out_buff(stderr, "test2"));
        return ret;
    }
    std::ostream& cerr() {
        //static std::ostream ret(new out_buff(stdclog, "test3"));
        return clog();//ret;
    }
}
*/
int main( int argc, char* argv[] ) {
    openqu::mpi::Environment env(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}

//#define CATCH_CONFIG_MAIN
//#include "catch2/catch.hpp"
