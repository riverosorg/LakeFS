// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <string>
#include <unordered_map>

auto etc_conf_reader(std::string path) -> std::unordered_map<std::string, std::string>;

// Exposed for testing
auto parse_config_line(std::string line) -> std::pair<std::string, std::string>;