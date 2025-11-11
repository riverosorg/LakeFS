// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "utilities.hpp"

TEST_CASE("Query Extraction", "[utilities]") {
    SECTION("Simple Query") {
        auto res = 
            extract_query("/test/wow/l/(tag1 | tag2)");

        CHECK(res == "(tag1 | tag2)");

        res = 
            extract_query("/test/wow/l/(tag1 | tag2)/file");

        CHECK(res == "(tag1 | tag2)");
    }

    SECTION("Subquerying") {
        auto res = 
            extract_query("/test/wow/l/(tag1 & (!place))");

        CHECK(res == "(tag1 & (!place))");

        res = 
            extract_query("/test/wow/l/(tag1 & (!place))/file");

        CHECK(res == "(tag1 & (!place))");
    }

    SECTION("Multilevel Subquerying") {
        auto res = 
            extract_query("/test/wow/l/(tag1 & ((!place) | (tag2 & tag3)))");

        CHECK(res == "(tag1 & ((!place) | (tag2 & tag3)))");

        res = 
            extract_query("/test/wow/l/(tag1 & ((!place) | (tag2 & tag3)))/file");

        CHECK(res == "(tag1 & ((!place) | (tag2 & tag3)))");
    }
}

TEST_CASE("String Splitting", "[utilities]") {
    SECTION("Single char delim") {
        const auto haystack = "wow/this/is/a/path/";
        const auto needle = "/";

        const auto found = split(haystack, needle);

        CHECK(found.size() == 5);
        CHECK(found[0] == "wow");
        CHECK(found[4] == "path");
    }

    SECTION("Multi char delim") {
        const auto haystack = "><i><love><testing";
        const auto needle = "><";

        const auto found = split(haystack, needle);
        
        CHECK(found.size() == 3);
        CHECK(found[0] == "i");
        CHECK(found[2] == "testing");
    }
} 