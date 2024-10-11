// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>
#include <vector>

#include "ast.hpp"

int db_init(const std::string db_file_path);
int db_close();

int db_add_file(const std::string path);
int db_tag_file(const std::string path, const std::string tag);

void db_set_default_query(const std::string query);

std::vector<std::string> db_run_query(const std::shared_ptr<AstNode> ast);
std::vector<std::string> db_run_default_query();

// Exposed for testing
std::string db_create_query(const std::shared_ptr<AstNode> ast_list);