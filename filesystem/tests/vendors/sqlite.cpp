// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <sqlite3.h>

TEST_CASE("SQLite3 Initialization", "[vendor]") {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);

    REQUIRE(rc == SQLITE_OK);

    sqlite3_close(db);
}

TEST_CASE("SQLite3 Querying", "[vendor]") {
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);

    REQUIRE(rc == SQLITE_OK);

    rc = sqlite3_exec(db, "CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT);", nullptr, nullptr, nullptr);

    REQUIRE(rc == SQLITE_OK);

    rc = sqlite3_exec(db, "INSERT INTO test (name) VALUES ('Test!');", nullptr, nullptr, nullptr);

    REQUIRE(rc == SQLITE_OK);

    // Prepare the SQL statement
    sqlite3_stmt *stmt;
    const char *sql = "SELECT name FROM test;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    REQUIRE(rc == SQLITE_OK);

    // Execute the prepared statement
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve the value of the "name" column
        const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        
        REQUIRE(std::string(name) == "Test!");
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}