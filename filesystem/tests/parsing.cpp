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
    std::cout << tokens << std::endl;
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

    ast = parse("a&b");
    ast = parse("a&(b|c)");
}
