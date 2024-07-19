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

    rc = sqlite3_exec(db, "INSERT INTO data (id, path) VALUES (1, '/testfile');", nullptr, nullptr, nullptr);
    rc = sqlite3_exec(db, "INSERT INTO tags (data_id, tag_value) VALUES (1, 'default');", nullptr, nullptr, nullptr);

    return rc;
}

int db_add_file(const std::string path) {
    int rc = sqlite3_exec(db, ("INSERT INTO data (path) VALUES ('" + path + "');").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

int db_tag_file(const std::string path, const std::string tag) {
    int rc = sqlite3_exec(db, ("INSERT INTO tags (data_id, tag_value) VALUES ((SELECT id FROM data WHERE path = '" + path + "'), '" + tag + "');").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

std::vector<std::string> db_tmp_query() {
    std::vector<std::string> results;

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT path FROM data WHERE id IN (SELECT data_id FROM tags WHERE tag_value = 'default');", -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
    }

    sqlite3_finalize(stmt);

    return results;
}