/**
 * @file load_corpus.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Loads the database corpus data for representation with quantum states
 * @version 0.1
 * @date 2019-03-11
 */

#include "corpus_utils.hpp"
#include "db_helper.hpp"

using namespace QNLP;

//If no filename is given, choose the database name from the macro/compiler defined value
CorpusUtils::CorpusUtils() : 
    db_helper(QNLP_DB_FILE), database_filename(QNLP_DB_FILE) {}

//Otherwise, take the given string name
CorpusUtils::CorpusUtils(const std::string filename) : 
    db_helper(filename), database_filename(filename) {}

CorpusUtils::~CorpusUtils() {
    CorpusUtils::clearData();
}

CorpusUtils::NTB& CorpusUtils::getNameToBin(){
    return this->name_to_bin;
}

CorpusUtils::BTN& CorpusUtils::getBinToName(){
    return this->bin_to_name;
}

void CorpusUtils::loadData(const std::string data_type, const std::string table){
    int rc;
    sqlite3_stmt *select_stmt = NULL;
    db_helper.openDB(database_filename);

    std::string query = "select name, bin_id from " + table + " where type=?";
    SQL_RETURN_CHECK ( sqlite3_prepare_v2( getDBHelper().getDBRef(), query.c_str(), -1, &select_stmt, NULL ), SQLITE_OK );

    //Arg 2 is the 1-based index of the variable to replace
    //Arg 4 is used to give the number of bytes, where -1 takes the length of string
    //Arg 5 specifies the destructor type, where SQLITE_STATIC causes it to ignore.
    SQL_RETURN_CHECK( sqlite3_bind_text( select_stmt, 1, data_type.c_str(), -1, SQLITE_STATIC ), SQLITE_OK );

    //Iterate through the rows of returned values
    while ( (rc = sqlite3_step(select_stmt)) == SQLITE_ROW) {
        std::string k( reinterpret_cast<const char*> ( sqlite3_column_text(select_stmt, 0) ) );
        unsigned int v = (unsigned int) sqlite3_column_int(select_stmt, 1);
        name_to_bin[data_type][k] = v;
        bin_to_name[data_type][v] = k;
    }
    SQL_RETURN_CHECK( sqlite3_finalize(select_stmt), SQLITE_OK );
}

void CorpusUtils::printData(const std::string type, const std::string table="corpus"){
    if(! this->name_to_bin.size()){
        loadData(type, table);
    }
    for(auto& dat : this->name_to_bin[type]){
        std::cout << dat.first << " : " << dat.second << std::endl;
    }
}

const std::string CorpusUtils::get_database_filename(){
    return this->database_filename;
}

void CorpusUtils::clearData(){
    this->bin_to_name.clear();
    this->name_to_bin.clear();
}

DBHelper& CorpusUtils::getDBHelper(){
    return this->db_helper;
}
