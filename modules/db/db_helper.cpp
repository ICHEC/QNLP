/**
 * @file db_helper.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Database access and manipulation functions
 * @version 0.1
 * @date 2019-03-11
 */

#include "db_helper.hpp"

using namespace QNLP;

DBHelper::DBHelper() : db_ptr(nullptr){};

DBHelper::DBHelper(const std::string filename) : db_ptr(nullptr) {
    std::cout << this->db_ptr << std::endl;
    SQL_RETURN_CHECK ( DB::openDB(filename), SQLITE_OK );
}

DBHelper::~DBHelper(){
    DBHelper::closeDB();
}

void DBHelper::closeDB(){
    if(this->db_ptr != nullptr ){
        std::cout << "CLOSING " << QNLP_DB_FILE << std::endl;
        SQL_RETURN_CHECK ( sqlite3_close(this->db_ptr), SQLITE_OK );
    }
}

int DBHelper::openDB(std::string filename){
    if(this->db_ptr == nullptr){ 
        std::cout << "OPENING " << filename << std::endl;
        return sqlite3_open( (const char *) filename.c_str(), &this->db_ptr ); 
    } else {
        return 0;
    }
}

sqlite3* DBHelper::getDBRef(){
    return this->db_ptr;
}
