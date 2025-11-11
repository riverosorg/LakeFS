// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <expected>
#include <string>
#include <vector>

// Runs a query normally. Handles special logic to pull out folders
std::expected<std::vector<std::string>, int> get_files(const char* path);

// Takes a query path that involves a tag query and returns the real FS path
std::expected<std::string, int> reverse_query(const char* path);

// Takes a path and returns the query segment
std::string extract_query(const char* path);

// Split a string based on a delimeter
std::vector<std::string> split(const std::string str, const std::string delim);