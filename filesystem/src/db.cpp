// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <iterator>
#include <optional>
#include <string>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include "db.hpp"
#include "query_lang/parser.hpp"

std::string default_query = "default";

// The global database connection.
// Perhaps better served by a singleton pattern.
static sqlite3 *db;

// Initializes the DB in memory
int db_tmp_init() {
    int rc = sqlite3_open(":memory:", &db);

    rc = sqlite3_exec(db, "CREATE TABLE tags (data_id INTEGER, tag_value TEXT);", nullptr, nullptr, nullptr);

    rc = sqlite3_exec(db, "CREATE TABLE data (id INTEGER PRIMARY KEY, path TEXT);", nullptr, nullptr, nullptr);

    return rc;
}

int db_init(const std::string db_file_path) {
    // check if DB file exists before opening
    const bool db_exists = std::filesystem::exists(db_file_path + "/current.db");

    int rc = sqlite3_open((db_file_path + "/current.db").c_str(), &db);

    if (db_exists) {
        return rc;
    }

    // Setting up the DB tables if it doesnt already exist
    rc = sqlite3_exec(db, "CREATE TABLE tags (data_id INTEGER, tag_value TEXT);", nullptr, nullptr, nullptr);

    rc = sqlite3_exec(db, "CREATE TABLE data (id INTEGER PRIMARY KEY, path TEXT);", nullptr, nullptr, nullptr);

    return rc;
}

int db_close() {
    int rc = sqlite3_close(db);

    return rc;
}

int db_create_backup(const std::string backup_path) {
    sqlite3* backup_db;
    if (sqlite3_open(backup_path.c_str(), &backup_db) != SQLITE_OK) {
        spdlog::error("Could not open new db at {0} for backup", backup_path);
        return -1;
    }

    sqlite3_backup* backup = NULL;

    // Will fail with NULL until we can aquire the read lock
    while (backup == NULL) {
        backup = sqlite3_backup_init(backup_db, "main", db, "main");

        // spdlog::error("Could not initialize backup: {0}", sqlite3_errmsg(backup_db));
    }

    auto remaining_pages = 1;

    while (remaining_pages > 0) {
        sqlite3_backup_step(backup, remaining_pages);

        remaining_pages = sqlite3_backup_remaining(backup);
        
        spdlog::debug("Remaining pages to backup: {0}", remaining_pages);
    }

    sqlite3_backup_finish(backup);
    sqlite3_close(backup_db);

    return 0;
}

int db_add_file(const std::string path) {
    int rc = sqlite3_exec(db, ("INSERT INTO data (path) VALUES ('" + path + "');").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

int db_tag_file(const std::string path, const std::string tag) {
    int rc = sqlite3_exec(db, ("INSERT INTO tags (data_id, tag_value) VALUES ((SELECT id FROM data WHERE path = '" + path + "'), '" + tag + "');").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

int db_remove_file(const std::string path) {
    int rc = sqlite3_exec(db, ("DELETE FROM tags WHERE data_id = (SELECT id FROM data WHERE path = '" + path + "');").c_str(), nullptr, nullptr, nullptr);
    rc = sqlite3_exec(db, ("DELETE FROM data WHERE path = '" + path + "';").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

int db_remove_tag(const std::string path, const std::string tag) {
    int rc = sqlite3_exec(db, ("DELETE FROM tags WHERE data_id = (SELECT id FROM data WHERE path = '" + path + "') AND tag_value = '" + tag + "';").c_str(), nullptr, nullptr, nullptr);

    return rc;
}

int db_relink_file(const std::string path, const std::string new_path) {
    int rc = sqlite3_exec(db, ("UPDATE data SET path = '" + new_path + "' WHERE data.path = '" + path + "';").c_str(), nullptr, nullptr, nullptr);
    
    return rc;
}

// Recursively generate a SQL query string for WHERE clause of the standard query
std::optional<std::string> db_query_helper(const std::shared_ptr<AstNode> ast) {
    std::string query_part;

    // Determine AST type
    if (auto tag = std::dynamic_pointer_cast<Tag>(ast)) {
        query_part += "SELECT data_id FROM tags WHERE tag_value = ";
        query_part += "'" + tag->name + "'";
    
    } else if (auto union_op = std::dynamic_pointer_cast<Union>(ast)) {
        query_part += "IN (";
        auto tmp_part = db_query_helper(union_op->left_node);

        if (!tmp_part.has_value()) {
            return tmp_part;
        }

        query_part += tmp_part.value();
        query_part += " UNION ";
        tmp_part = db_query_helper(union_op->right_node);

        if (!tmp_part.has_value()) {
            return tmp_part;
        }

        query_part += tmp_part.value();
    
    } else if (auto intersection_op = std::dynamic_pointer_cast<Intersection>(ast)) {
        query_part += "IN (";
        auto tmp_part = db_query_helper(intersection_op->left_node);

        if (!tmp_part.has_value()) {
            return tmp_part;
        }

        query_part += tmp_part.value();
        query_part += ") AND id IN (";
        tmp_part = db_query_helper(intersection_op->right_node);

        if (!tmp_part.has_value()) {
            return tmp_part;
        }

        query_part += tmp_part.value();

    } else if (auto negation_op = std::dynamic_pointer_cast<Negation>(ast)) {
        query_part += "NOT IN (";
        auto tmp_part = db_query_helper(negation_op->node);

        if (!tmp_part.has_value()) {
            return tmp_part;
        }

        query_part += tmp_part.value();
    
    } else {
        return {};
    }
     
    return query_part;
}

// Creates a syntactically valid SQLite3 query from an ASTNode
std::optional<std::string> db_create_query(const std::shared_ptr<AstNode> ast) {
    std::string query;

    // Tag selection preamble
    query += "SELECT path ";
    query += "FROM data WHERE id "; 
    //IN (SELECT data_id FROM tags WHERE tag_value ";

    if  (auto tag = std::dynamic_pointer_cast<Tag>(ast)){
        // TODO: This is a special case and i hate it
        query += "IN (SELECT data_id FROM tags WHERE tag_value ";
        query += "= '" + tag->name + "'";
    
    } else {
        const auto query_part = db_query_helper(ast);

        if (!query_part.has_value()) {
            return {};
        }

        query += query_part.value();
    }

    query += ");";

    return query;
}

void db_set_default_query(const std::string query) {
    default_query = query;
}

// TODO: I dont want the DB owning the default query, this was just easy for now
// (im sleepy, sorry future me)
std::vector<std::string> db_run_default_query() {
    const auto query_ast = parse(default_query);

    if (query_ast.has_value()) {
        return db_run_query(query_ast.value());
    }

    return {};
}

// TODO: return a file struct containing the unique file ID
std::vector<std::string> db_run_query(const std::shared_ptr<AstNode> ast) {
    std::vector<std::string> results;

    auto query = db_create_query(ast);

    if (!query.has_value()) {
        return results;
    }

    spdlog::debug("Running Query: \n{0}\n", query.value());

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query.value().c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
    }

    sqlite3_finalize(stmt);

    return results;
}