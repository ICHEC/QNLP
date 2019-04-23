/**
 * @file read_tags.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Reads the tagged corpus data for representation with quantum states
 * @version 0.1
 * @date 2019-03-11
 */

#include "../include/read_tags.hpp"

using namespace QNLP;

ReadTags::ReadTags() : DB(nullptr) {
    std::cout << this->DB << std::endl;
    SQL_RETURN_CHECK ( ReadTags::openDB(), SQLITE_OK );
}

ReadTags::~ReadTags(){
    ReadTags::closeDB();
}

void ReadTags::closeDB(){
    if(this->DB != nullptr ){
        std::cout << "CLOSING " << QNLP_DB_FILE << std::endl;
        SQL_RETURN_CHECK ( sqlite3_close(this->DB), SQLITE_OK );
    }
}

int ReadTags::openDB(){
    if(this->DB == nullptr){ 
        std::cout << "OPENING " << QNLP_DB_FILE << std::endl;
        return sqlite3_open( (const char *) QNLP_DB_FILE, &this->DB ); 
    } else {
        return 0;
    }
}

void ReadTags::loadData(std::string data_type){
    int rc;
    sqlite3_stmt *select_stmt = NULL;

    ReadTags::openDB();
    //std::string query = "SELECT name, bin_id FROM qnlp WHERE type=?";
    std::string query = "select name, bin_id from qnlp where type=?";
    SQL_RETURN_CHECK ( sqlite3_prepare_v2( this->DB, query.c_str(), -1, &select_stmt, NULL ), SQLITE_OK );

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

ReadTags::NTB& ReadTags::getNameToBin(){
    return this->name_to_bin;
}

ReadTags::BTN& ReadTags::getBinToName(){
    return this->bin_to_name;
}

void ReadTags::printData(std::string type){
    if(! this->name_to_bin.size()){
        ReadTags::loadData(type);
    }
    for(auto& dat : this->name_to_bin[type]){
        std::cout << dat.first << " : " << dat.second << std::endl;
    }
}

sqlite3* ReadTags::getDBRef(){
    return this->DB;
}
