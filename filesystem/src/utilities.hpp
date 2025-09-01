// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <string>

// Takes a query path that involves a tag query and returns the real FS path
std::string reverse_query(const char* path);

// Takes a path and returns the query segment
std::string extract_query(const char* path);