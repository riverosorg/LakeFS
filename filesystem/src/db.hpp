// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>
#include <vector>
#include <optional>

#include "query_lang/ast.hpp"

int db_init(const std::string db_file_path);
int db_tmp_init();
int db_close();

int db_create_backup(const std::string backup_path);

int db_add_file(const std::string path);
int db_tag_file(const std::string path, const std::string tag);
int db_remove_file(const std::string path);
int db_remove_tag(const std::string path, const std::string tag);
int db_relink_file(const std::string path, const std::string new_path);

void db_set_default_query(const std::string query);

std::vector<std::string> db_run_query(const std::shared_ptr<AstNode> ast);
std::vector<std::string> db_run_default_query();

// Exposed for testing
std::optional<std::string> db_create_query(const std::shared_ptr<AstNode> ast_list);