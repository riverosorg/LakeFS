// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "query_lang/ast.hpp"
#include "query_lang/parser.hpp"
#include "db.hpp"

// TODO: testing direct string queries like this will be fragile. A SQL AST
// Will be more robust and easier to translate to a different DB in the future

TEST_CASE("Basic Tag Retrieval", "[parsing][query]") {
    std::shared_ptr<AstNode> ast;
    std::string query;
    std::string expected_query;

    SECTION("Single Tag Retrieval") {
        ast = parse("tag");
        query = db_create_query(ast);
        expected_query = 
            "SELECT path FROM data WHERE id IN "
            "(SELECT data_id FROM tags WHERE tag_value = 'tag');";
        
        REQUIRE(query == expected_query);
    }

    SECTION("Negation") {
        ast = parse("!tag");
        query = db_create_query(ast);
        expected_query = 
            "SELECT path FROM data WHERE id NOT IN "
            "(SELECT data_id FROM tags WHERE tag_value = 'tag');";
        
        REQUIRE(query == expected_query);
    }

    SECTION("Union Retrieval") {
        ast = parse("tag1|tag2");
        query = db_create_query(ast);
        expected_query = 
            "SELECT path FROM data WHERE id IN "
            "(SELECT data_id FROM tags WHERE tag_value = 'tag1' UNION SELECT data_id FROM tags WHERE tag_value = 'tag2');";
        REQUIRE(query == expected_query);
    }

    SECTION("Intersection Retrieval") {
        ast = parse("tag1&tag2");
        query = db_create_query(ast);
        expected_query = 
            "SELECT path FROM data WHERE id IN "
            "(SELECT data_id FROM tags WHERE tag_value = 'tag1') "
            "AND id IN "
            "(SELECT data_id FROM tags WHERE tag_value = 'tag2');";
        
        REQUIRE(query == expected_query);
    }

    SECTION("Issue #34: Potential Runtime error") {
        ast = parse("((tag1 | tag2) &)");

        REQUIRE_NOTHROW(db_create_query(ast));
    }
}