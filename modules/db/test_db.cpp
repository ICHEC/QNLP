#include "db_helper.hpp"
#include "corpus_utils.hpp"
#include "Catch2/single_include/catch2/catch.hpp"

#define CATCH_CONFIG_MAIN
using namespace QNLP;

/**
 * @brief Unit tests for the DBHelper class
 * 
 */
SECTION("DBHelper unit tests"){

    /**
     * @brief Testing default constructor and subsequent functions on created object
     * 
     */
    SECTION("Default constructor: DBHelper()"){
        DBHelper db;

        REQUIRE(db.getFilename() == "");
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
     * @brief Testing parameterised constructor and subsequent functions on created object
     * 
     */
    SECTION("Parameterised constructor: DBHelper(const std::string filename) with 'test.db'"){
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
     * @brief Testing parameterised constructor and subsequent functions on created object with no database found
     * 
     */
    SECTION("Parameterised constructor: DBHelper(const std::string filename) with 'none.db'"){
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
}

/**
 * @brief Unit tests for the CorpusUtils class
 * 
 */
SECTION("CorpusUtils unit tests"){

}