// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>
#include <vector>

// Takes a query path that involves a tag query and returns the real FS path
std::string reverse_query(const char* path);

// Takes a path and returns the query segment
std::string extract_query(const char* path);

// Split a string based on a delimeter
std::vector<std::string> split(const std::string str, const std::string delim);