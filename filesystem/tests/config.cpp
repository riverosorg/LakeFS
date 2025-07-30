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

TEST_CASE("Interval Parsing", "[config]") {

    // Conversions
    const auto seconds = 60;
    const auto minutes = 60;
    const auto hours = 24;
    const auto days = 7;
    const auto weeks = 4;


    SECTION("Hours") {
        auto interval = parse_interval_value("3 hours");
        CHECK(interval.count() == 3 * minutes * seconds);
    }

    SECTION("Days") {
        auto interval = parse_interval_value("5 days");
        CHECK(interval.count() == 5 * hours * minutes * seconds);
    }

    SECTION("Weeks") {
        auto interval = parse_interval_value("2 weeks");
        CHECK(interval.count() == 2 * days * hours * minutes * seconds);
    }

    SECTION("Months") {
        auto interval = parse_interval_value("3 months");
        CHECK(interval.count() == 3 * 2629746); // NOTE: std::chronos ratio for months is this magic value, which is I assume more accurate than the naive calculation
    }
}