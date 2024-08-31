// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "parser.hpp"

TEST_CASE("Simple Token operators", "[parsing]") {
    REQUIRE(Token("abc123") == Token("abc123"));
    REQUIRE_FALSE(Token("abc") == Token("123"));
}

TEST_CASE("Tokenizing", "[parsing]") {
    std::vector<Token> tokens;

    tokens = tokenize("abc123");
    REQUIRE(tokens == std::vector<Token>{
        Token("abc123"),
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

    tokens = tokenize("two_words");
    REQUIRE(tokens == std::vector<Token>{
        Token("two_words"),
    });
}


TEST_CASE("Parsing text into a AST query", "[parsing]") {
    AstNode *ast;
    AstNode *expected_ast;

    SECTION("Single Letter Tags") {
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

        ast = parse("(b|c)&a");
        expected_ast = new Intersection(
            new Union(
                new Tag("b"),
                new Tag("c")
            ),
            new Tag("a")
        );
        REQUIRE(ast->match(expected_ast));
    }

    SECTION("Word Tags") {
        ast = parse("tag1&tag2");
        expected_ast = new Intersection(
            new Tag("tag1"),
            new Tag("tag2")
        );
        REQUIRE(ast->match(expected_ast));

        ast = parse("tag1&(tag2|tag3)");
        expected_ast = new Intersection(
            new Tag("tag1"),
            new Union(
                new Tag("tag2"),
                new Tag("tag3")
            )
        );
        REQUIRE(ast->match(expected_ast));
    }

    SECTION("Complex Queries") {
        ast = parse("(picture|video)&(year_2014&(!digital))");
        expected_ast = new Intersection(
            new Union(
                new Tag("picture"),
                new Tag("video")
            ),
            new Intersection(
                new Tag("year_2014"),
                new Negation(
                    new Tag("digital")
                )
            )
        );

        REQUIRE(ast->match(expected_ast));
    }
}
