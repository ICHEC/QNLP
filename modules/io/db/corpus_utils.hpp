/**
 * @file load_corpus.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Loads the database corpus data for representation with quantum states
 * @version 0.1
 * @date 2019-03-11
 */
#ifndef QNLP_CORPUS_LOADER
#define QNLP_CORPUS_LOADER

#include <unordered_map>
#include "db_helper.hpp"

#ifndef QNLP_DB_FILE
#define QNLP_DB_FILE "qnlp_tagged_corpus.sqlite"
#endif

namespace QNLP{
    class CorpusUtils{
    public:

        /**
         * @brief Construct a new Load Corpus object. Database name taken from the macro/compiler defined value
         * 
         */
        CorpusUtils();
        /**
         * @brief Construct a new Load Corpus object. Database name given explicitly.
         * 
         * @param filename Database filename
         */
        CorpusUtils(const std::string filename);
        ~CorpusUtils();

        typedef std::unordered_map<std::string, std::unordered_map<std::string, unsigned int> > NTB;
        typedef std::unordered_map<std::string, std::unordered_map<unsigned int, std::string> > BTN;

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
         * @brief Loads from the database for a given data type (noun, verb, etc)
         * 
         * @param data_type String of the data type to load
         * @param table Database table to load data from
         */
        void loadData(const std::string data_type, const std::string table="corpus");
        /**
         * @brief Return reference to database object 
         * 
         * @return DB& reference to database object
         */
        DBHelper& getDBHelper();

        /**
         * @brief Print the data with given type
         * 
         * @param type Grammatical type
         * @param table Database table to print data from
         */
        void printData(const std::string type, const std::string table);

        /**
         * @brief Get the database filename object
         * 
         * @return const std::string 
         */
        const std::string get_database_filename();

        /**
         * @brief Clear all loaded data
         * 
         */
        void clearData();

    private:
        //Maps for strings to binary pattern and reverse maps. Outer map keys are grammatical
        //types (nouns, verbs, etc.). Returned maps then hold unique entries. 
        NTB name_to_bin;
        BTN bin_to_name;

        //Database helper boject
        DBHelper db_helper;
        std::string database_filename;
    };
}

#endif