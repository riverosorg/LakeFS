// SPDX-FileCopyrightText: 2024 Conner Tenn
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "parser.hpp"

TEST_CASE("Sanity test", "[vendor]") {
    REQUIRE(1 == 1);
}

TEST_CASE("Token", "[vendor]") {
    REQUIRE(Token("abc123") == Token("abc123"));
    REQUIRE_FALSE(Token("abc") == Token("123"));
}

TEST_CASE("Tokenize", "[vendor]") {
    std::vector<Token> tokens;

    tokens = tokenize("abc123");
    REQUIRE(tokens == std::vector<Token>{
        Token("abc123")
    });

    tokens = tokenize("abc&123+(d)");
    REQUIRE(tokens == std::vector<Token>{
        Token("abc"),
        Token("&"),
        Token("123"),
        Token("+"),
        Token("("),
        Token("d"),
        Token(")"),
    });
}


TEST_CASE("parse", "[vendor]") {
    AstNode *ast;
    AstNode *expected_ast;

    ast = parse("a&b");
    expected_ast = new Intersection(
        new Tag("a"),
        new Tag("b")
    );
    REQUIRE(ast->match(expected_ast));

    ast = parse("a&(b|c)");
    expected_ast = new Intersection(
        new Tag("a"),
        new Union(
            new Tag("b"),
            new Tag("c")
        )
    );
    REQUIRE(ast->match(expected_ast));
}
