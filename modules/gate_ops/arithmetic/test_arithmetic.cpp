/**
 * @file test_arithmetic.cpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Unit tests for the 'arithmetic' module. Classes tested: [SumSub]
 * @version 0.1
 * @date 2019-05-01
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#define CATCH_CONFIG_MAIN

#include "sum_sub.hpp"
#include "catch2/catch.hpp"

using namespace QNLP;

/**
 * @brief Testing default constructor and subsequent functions on DBHelper class
 * 
 */
TEST_CASE("SumSub unit tests: Default constructor: SumSub()","[arithmetic]"){

    REQUIRE( 1 != 0 );
    REQUIRE( 0 == 0 );

    SECTION("Section Label 1"){
        SECTION("Section Label 1.1"){
        }
    }
}
