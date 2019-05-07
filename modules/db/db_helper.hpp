/**
 * @file db_helper.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Database access and manipulation functions
 * @version 0.1
 * @date 2019-03-11
 */

#ifndef QNLP_DBHELPER
#define QNLP_DBHELPER

#include <iostream> 
#include <sqlite3.h> 
#include <string> 

//#############################################################################
 
//Return-type checking macro for SQL statements; compares the result x with that of y, and exits with the respective error-code if they differ. Macro wraps inline to return linenumber and filename for resultant cause.
#define SQL_RETURN_CHECK(x,y) sql_return_check(x, y, __FILE__, __LINE__)
inline void sql_return_check(   const int ret_code, 
                                const int expected_code, 
                                const char* fileName, 
                                const std::size_t lineNum){
#ifdef SQL_ERR_CHECK
    if ( ret_code != expected_code ) { 
        std::cerr << "SQL error: FILE: " << fileName << "\t LINE: " << lineNum << "\t RETURN_CODE: " << ret_code << std::endl; 
        std::exit(ret_code); 
    }
#endif
}

//#############################################################################

namespace QNLP{
    class DBHelper{
    public:

        DBHelper();
        DBHelper(const std::string filename) ;
        ~DBHelper();

        /**
         * @brief Opens the requested database, and gives the return code of the operation. If the DB pointer has already been associated with data, then the return code is 0, and no operation is performed.
         * 
         * @param string Path to database on filesystem.
         * @return int Return code of the DB access.
         */
        int openDB(const std::string filename);

        /**
         * @brief Closes the database if the DB pointer is open.
         */
        void closeDB();

        /**
         * @brief Returns a pointer to the DB object
         * 
         * @return sqlite3* 
         */
        sqlite3* getDBRef();

        /**
         * @brief Get the name of the opened database
         * 
         * @return std::string 
         */
        std::string getFilename();

    private:
        //Pointer to database object
        sqlite3* db_ptr;
        std::string db_filename;
    };
}
#endif
