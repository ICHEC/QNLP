/**
 * @file test_db.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Unit tests for the 'db' module. Classes tested: [DBHelper, CorpusUtils]
 * @version 0.1
 * @date 2019-05-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "db_helper.hpp"
#include "corpus_utils.hpp"
#include "Catch2/single_include/catch2/catch.hpp"

#define CATCH_CONFIG_MAIN
using namespace QNLP;

/**
 * @brief Testing default constructor and subsequent functions on DBHelper class
 * 
 */
TEST_CASE("DBHelper unit tests: Default constructor: DBHelper()","[db]"){
    DBHelper db;

    REQUIRE(db.getFilename() == "qnlp_tagged_corpus.sqlite");
    REQUIRE(db.getDBRef() == nullptr);

    SECTION("openDB(const std::string filename) with 'test.db'"){
        int ret_code = db.openDB("test.db");
        REQUIRE(ret_code == 0);
        REQUIRE(db.getDBRef() != nullptr);
        REQUIRE(db.getFilename().compare("test.db") == 0);

        SECTION("closeDB()"){
            db.closeDB();
            REQUIRE(db.getFilename() == "");
            REQUIRE(db.getDBRef() == nullptr );
        }
    }
    SECTION("openDB(const std::string filename) with 'none.db'"){
        int ret_code = db.openDB("none.db");
        REQUIRE(ret_code != 0);
        REQUIRE(db.getDBRef() == nullptr);
        REQUIRE(db.getFilename().compare("none.db") == 0);

        SECTION("closeDB()"){
            db.closeDB();
            REQUIRE(db.getFilename() == "");
            REQUIRE(db.getDBRef() == nullptr );
        }
    }
}
    
/**
 * @brief Testing parameterised constructor and subsequent functions on DBHelper 
 * class with given database 'test.db'
 * 
 */
TEST_CASE("Parameterised constructor: DBHelper(const std::string filename) with 'test.db'","[db]"){
    DBHelper db("test.db");

    REQUIRE(db.getFilename() == "test.db");
    REQUIRE(db.getDBRef() != nullptr);

    SECTION("openDB(const std::string filename) with 'test.db'"){
        int ret_code = db.openDB("test.db");
        REQUIRE(ret_code == 0);
        REQUIRE(db.getDBRef() != nullptr);
        REQUIRE(db.getFilename().compare("test.db") == 0);
    }
    SECTION("closeDB()"){
        db.closeDB();
        REQUIRE(db.getFilename() == "");
        REQUIRE(db.getDBRef() == nullptr );
    }
}
    
/**
 * @brief Testing parameterised constructor and subsequent functions on DBHelper 
 * class with non-existant database 'none.db'
 * 
 */
TEST_CASE("Parameterised constructor: DBHelper(const std::string filename) with 'none.db'","[db]"){
    DBHelper db("none.db");

    REQUIRE(db.getFilename() == "none.db");
    REQUIRE(db.getDBRef() == nullptr);

    SECTION("openDB(const std::string filename) with 'none.db'"){
        int ret_code = db.openDB("none.db");
        REQUIRE(ret_code != 0);
        REQUIRE(db.getDBRef() == nullptr);
        REQUIRE(db.getFilename().compare("test.db") == 0);
    }
    SECTION("closeDB()"){
        db.closeDB();
        REQUIRE(db.getFilename() == "");
        REQUIRE(db.getDBRef() == nullptr );
    }
}

/**
 * @brief Testing default constructor and subsequent functions on CorpusUtils class
 * 
 */
TEST_CASE("CorpusUtils unit tests: Default constructor: CorpusUtils()","[db]"){
    CorpusUtils cu;
    
    REQUIRE(cu.getBinToName().size() == 0);
    REQUIRE(cu.getNameToBin().size() == 0);
    REQUIRE(cu.get_database_filename().compare("") == 0);
    REQUIRE(cu.getDBHelper().getFilename().compare("qnlp_tagged_corpus.sqlite") == 0);
    REQUIRE(cu.getDBHelper().getDBRef() == nullptr);

    SECTION("Load data: data_type=noun, table is default ('corpus')"){
        cu.loadData("noun");
    }

}