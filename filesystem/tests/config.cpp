// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#include "config.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Config Parsing", "[config]") {

    SECTION("Simple Key Value") {
        auto kv_pair = parse_config_line("test=1");
        CHECK(kv_pair.first == "test");
        CHECK(kv_pair.second == "1");
    }

    SECTION("Key Value with spaces") {
        auto kv_pair = parse_config_line("key name=value is something");
        CHECK(kv_pair.first == "key name");
        CHECK(kv_pair.second == "value is something");
    }
}