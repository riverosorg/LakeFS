// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ast.hpp"
#include "parser.hpp"
#include "db.hpp"

// TODO: testing direct string queries like this will be fragile. A SQL AST
// Will be more robust and easier to translate to a different DB in the future

TEST_CASE("Basic Tag Retrieval", "[parsing][query]") {
    AstNode *ast;
    std::string query;
    std::string expected_query;

    SECTION("Single Tag Retrieval") {
        ast = parse("tag");
        query = db_create_query(ast);
        expected_query = 
            "SELECT path FROM data WHERE id IN (SELECT data_id FROM tags WHERE tag_value in ('tag'));";
        
        REQUIRE(query == expected_query);
    }
}