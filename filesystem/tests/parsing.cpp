// SPDX-FileCopyrightText: 2024 Conner Tenn
// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "query_lang/parser.hpp"

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
    std::shared_ptr<AstNode> ast;
    std::shared_ptr<AstNode> expected_ast;

    SECTION("Single Letter Tags") {
        ast = parse("a&b");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Tag>("a"),
            std::make_shared<Tag>("b")
        );
        REQUIRE(ast->match(expected_ast));

        ast = parse("a&(b|c)");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Tag>("a"),
            std::make_shared<Union>(
                std::make_shared<Tag>("b"),
                std::make_shared<Tag>("c")
            )
        );
        REQUIRE(ast->match(expected_ast));

        ast = parse("(b|c)&a");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Union>(
                std::make_shared<Tag>("b"),
                std::make_shared<Tag>("c")
            ),
            std::make_shared<Tag>("a")
        );
        REQUIRE(ast->match(expected_ast));
    }

    SECTION("Word Tags") {
        ast = parse("tag1&tag2");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Tag>("tag1"),
            std::make_shared<Tag>("tag2")
        );
        REQUIRE(ast->match(expected_ast));

        ast = parse("tag1&(tag2|tag3)");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Tag>("tag1"),
            std::make_shared<Union>(
                std::make_shared<Tag>("tag2"),
                std::make_shared<Tag>("tag3")
            )
        );
        REQUIRE(ast->match(expected_ast));
    }

    SECTION("Complex Queries") {
        ast = parse("(picture|video)&(year_2014&(!digital))");
        expected_ast = std::make_shared<Intersection>(
            std::make_shared<Union>(
                std::make_shared<Tag>("picture"),
                std::make_shared<Tag>("video")
            ),
            std::make_shared<Intersection>(
                std::make_shared<Tag>("year_2014"),
                std::make_shared<Negation>(
                    std::make_shared<Tag>("digital")
                )
            )
        );

        std::cout << ast->str() << std::endl;
        std::cout << expected_ast->str() << std::endl;

        REQUIRE(ast->match(expected_ast));
    }
}