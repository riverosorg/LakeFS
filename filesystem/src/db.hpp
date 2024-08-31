// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>
#include <vector>

#include "ast.hpp"

int db_init();

int db_add_file(const std::string path);
int db_tag_file(const std::string path, const std::string tag);

std::vector<std::string> db_run_query(const AstNode* ast);

// Exposed for testing
std::string db_create_query(const AstNode* ast_list);