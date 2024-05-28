// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "sqlite/sqlite3.h"

TEST_CASE("SQLite3 Initialization", "[vendor]") {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);

    REQUIRE(rc == SQLITE_OK);

    sqlite3_close(db);
}