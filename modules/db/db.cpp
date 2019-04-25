/**
 * @file db.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Loads the database corpus data for representation with quantum states
 * @version 0.1
 * @date 2019-03-11
 */

#include "db.hpp"

using namespace QNLP;

DB::DB() : db_ptr(nullptr) {
    std::cout << this->db_ptr << std::endl;
    SQL_RETURN_CHECK ( DB::openDB(), SQLITE_OK );
}

DB::~DB(){
    DB::closeDB();
}

void DB::closeDB(){
    if(this->db_ptr != nullptr ){
        std::cout << "CLOSING " << QNLP_DB_FILE << std::endl;
        SQL_RETURN_CHECK ( sqlite3_close(this->db_ptr), SQLITE_OK );
    }
}

int DB::openDB(){
    if(this->db_ptr == nullptr){ 
        std::cout << "OPENING " << QNLP_DB_FILE << std::endl;
        return sqlite3_open( (const char *) QNLP_DB_FILE, &this->db_ptr ); 
    } else {
        return 0;
    }
}

void DB::loadData(std::string data_type){
    int rc;
    sqlite3_stmt *select_stmt = NULL;

    DB::openDB();
    //std::string query = "SELECT name, bin_id FROM qnlp WHERE type=?";
    std::string query = "select name, bin_id from corpus where type=?";
    SQL_RETURN_CHECK ( sqlite3_prepare_v2( this->db_ptr, query.c_str(), -1, &select_stmt, NULL ), SQLITE_OK );

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

DB::NTB& DB::getNameToBin(){
    return this->name_to_bin;
}

DB::BTN& DB::getBinToName(){
    return this->bin_to_name;
}

void DB::printData(std::string type){
    if(! this->name_to_bin.size()){
        DB::loadData(type);
    }
    for(auto& dat : this->name_to_bin[type]){
        std::cout << dat.first << " : " << dat.second << std::endl;
    }
}

sqlite3* DB::getDBRef(){
    return this->db_ptr;
}
