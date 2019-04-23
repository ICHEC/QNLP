/**
 * @file read_tags.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Reads the tagged corpus data for representation with quantum states
 * @version 0.1
 * @date 2019-03-11
 */

#ifndef QNLP_READ_TAGS
#define QNLP_READ_TAGS

#include <iostream> 
#include <sqlite3.h> 
  
#include <string> 
#include <unordered_map> 

#ifndef QNLP_DB_FILE
#define QNLP_DB_FILE "qnlp_tagged_corpus.sqlite"
#endif

//#############################################################################
 
//Return-type checking macro for SQL statements; compares the result with that of SQLITE_OK, and exits with the respective error-code if they differ. Macro wraps inline to return linenumber and filename for resultant cause.
#define SQL_RETURN_CHECK(x,y) sql_return_check(x, y, __FILE__, __LINE__)
inline void sql_return_check(   const int ret_code, 
                                const int expected_code, 
                                const char* fileName, 
                                const std::size_t lineNum){
    if ( ret_code != expected_code ) { 
        std::cerr << "SQL error: FILE: " << fileName << "\t LINE: " << lineNum << "\t RETURN_CODE: " << ret_code << std::endl; 
        std::exit(ret_code); 
    }
}

//#############################################################################

namespace QNLP{
    class ReadTags{
    public:
        typedef std::unordered_map<std::string, std::unordered_map<std::string, unsigned int> > NTB;
        typedef std::unordered_map<std::string, std::unordered_map<unsigned int, std::string> > BTN;

        ReadTags();
        ~ReadTags();

        /**
         * @brief Get the NTB object. This is a map of maps, wherein the first key represents the grammatical type to load. The second key accesses the name to binary string mapping data, wherein each unique entity has a mapping to a unique binary string, which will representation it's encoding in the quantum state-space.
         * 
         * @return NTB& 
         */
        NTB& getNameToBin();
        /**
         * @brief Get the BTN object. This is a map of maps, wherein the first key represents the grammatical type to load. The second key accesses binary mapping data, wherein each unique entity has a mapping to a unique string entity. This allows for determining a result upon measurement of the quantum state-space back to a specific meaning eneity.
         * 
         * @return BTN& 
         */
        BTN& getBinToName();

        /**
         * @brief Opens the qnlp_tagged_corpus.sqlite database, and gives the return code of the operation. If the DB pointer has already been associated with data, then the return code is 0, and no operation is performed.
         * 
         * @return int Return code of the DB access.
         */
        int openDB();

        /**
         * @brief Closes the qnlp_tagged_corpus.sqlite database if the DB pointer is open.
         */
        void closeDB();
        /**
         * @brief Loads from the database for a given data type (noun, verb, etc)
         * 
         * @param data_type String of the data type to load
         */
        void loadData(std::string data_type);

        /**
         * @brief Print the data with given type
         * 
         * @param type Grammatical type
         */
        void printData(std::string type);

        /**
         * @brief Returns a pointer to the DB object
         * 
         * @return sqlite3* 
         */
        sqlite3* getDBRef();

    private:
        sqlite3* DB;

        //Maps for strings to binary pattern and reverse maps. Outer map keys are grammatical
        //types (nouns, verbs, etc.). Returned maps then hold unique entries. 
        NTB name_to_bin;
        BTN bin_to_name;
    };
}
#endif
