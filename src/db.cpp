// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "sqlite/sqlite3.h"
#include "db.hpp"

static const char* db_file_name = ":memory:";

// The global database connection.
// Perhaps better served by a singleton pattern.
static sqlite3 *db;

int db_init() {
    int rc = sqlite3_open(db_file_name, &db);

    // Temporarily hardcoding the DB

    rc = sqlite3_exec(db, "CREATE TABLE tags (data_id INTEGER, tag_value TEXT);", nullptr, nullptr, nullptr);

    rc = sqlite3_exec(db, "CREATE TABLE data (id INTEGER PRIMARY KEY, path TEXT);", nullptr, nullptr, nullptr);

    rc = sqlite3_exec(db, "INSERT INTO data (id, path) VALUES (1, '/main/iso/template/iso/FD13LIVE.iso');", nullptr, nullptr, nullptr);
    rc = sqlite3_exec(db, "INSERT INTO tags (data_id, tag_value) VALUES (1, 'default');", nullptr, nullptr, nullptr);

    return rc;
}