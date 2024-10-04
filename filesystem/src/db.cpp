// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include <string>

#include "sqlite/sqlite3.h"
#include "db.hpp"
#include "parser.hpp"

static const char* db_file_name = ":memory:";

std::string default_query = "default";

// The global database connection.
// Perhaps better served by a singleton pattern.
static sqlite3 *db;

int db_init() {
    int rc = sqlite3_open(db_file_name, &db);

    // Temporarily hardcoding the DB

    rc = sqlite3_exec(db, "CREATE TABLE tags (data_id INTEGER, tag_value TEXT);", nullptr, nullptr, nullptr);

    rc = sqlite3_exec(db, "CREATE TABLE data (id INTEGER PRIMARY KEY, path TEXT);", nullptr, nullptr, nullptr);

    return rc;
}

int db_close() {
    int rc = sqlite3_close(db);

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

// Recursively generate a SQL query string for WHERE clause of the standard query
std::string db_query_helper(const AstNode* ast) {
    std::string query_part;

    // Determine AST type
    if (auto tag = dynamic_cast<const Tag*>(ast)) {
        query_part += "'" + tag->name + "'";
    
    } else if (auto union_op = dynamic_cast<const Union*>(ast)) {
        query_part += "IN (";
        query_part += db_query_helper(union_op->left_node);
        query_part += ", ";
        query_part += db_query_helper(union_op->right_node);
        query_part += ")";
    
    } else if (auto intersection_op = dynamic_cast<const Intersection*>(ast)) {
        if  (auto tag = dynamic_cast<const Tag*>(intersection_op->left_node)){
            query_part += "= '" + tag->name + "'";
        
        } else {
            query_part += db_query_helper(intersection_op->left_node);
        }

        query_part += ") AND id IN (SELECT data_id FROM tags WHERE tag_value ";

        if  (auto tag = dynamic_cast<const Tag*>(intersection_op->right_node)){
            query_part += "= '" + tag->name + "'";
        
        } else {
            query_part += db_query_helper(intersection_op->right_node);
        }

    } else if (auto negation_op = dynamic_cast<const Negation*>(ast)) {
        query_part += "!= ";
        query_part += db_query_helper(negation_op->node);
    
    } else {
        throw std::runtime_error("Unknown AST type");
    }
     
    return query_part;
}

// Creates a syntactically valid SQLite3 query from an ASTNode
std::string db_create_query(const AstNode* ast) {
    std::string query;

    // Tag selection preamble
    query += "SELECT path ";
    query += "FROM data WHERE id IN (SELECT data_id FROM tags WHERE tag_value ";

    if  (auto tag = dynamic_cast<const Tag*>(ast)){
        // TODO: This is a special case and i hate it
        query += "= '" + tag->name + "'";
    
    } else {
        query += db_query_helper(ast);
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
    return db_run_query(parse(default_query));
}

// TODO: return a file struct containing the unique file ID
std::vector<std::string> db_run_query(const AstNode* ast) {
    std::vector<std::string> results;

    std::string query = db_create_query(ast);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
    }

    sqlite3_finalize(stmt);

    return results;
}