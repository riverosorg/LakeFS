// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>
#include <vector>

int db_init();

int db_add_file(const std::string path);
int db_tag_file(const std::string path, const std::string tag);

std::vector<std::string> db_tmp_query(); 