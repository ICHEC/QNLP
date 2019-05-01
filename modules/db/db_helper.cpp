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

DBHelper::DBHelper(const std::string filename) : db_ptr(nullptr), db_filename(filename){
    std::cout << this->db_ptr << std::endl;
    SQL_RETURN_CHECK ( DBHelper::openDB(filename), SQLITE_OK );
}

DBHelper::~DBHelper(){
    DBHelper::closeDB();
}

void DBHelper::closeDB(){
    if(this->db_ptr != nullptr ){
        std::cout << "CLOSING " << db_filename << std::endl;
        SQL_RETURN_CHECK ( sqlite3_close(this->db_ptr), SQLITE_OK );
    }
    db_filename.clear();
    db_ptr == nullptr;
}

int DBHelper::openDB(std::string filename){
    if(this->db_ptr == nullptr){
        this->db_filename = filename;
        std::cout << "OPENING " << filename << std::endl;
        return sqlite3_open( (const char *) filename.c_str(), &this->db_ptr ); 
    } else {
        return 0;
    }
}

sqlite3* DBHelper::getDBRef(){
    return this->db_ptr;
}

std::string DBHelper::getFilename(){
    return this->db_filename;
}
