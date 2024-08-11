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

