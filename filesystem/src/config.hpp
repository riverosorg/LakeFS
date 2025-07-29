// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <string>
#include <unordered_map>
#include <chrono>

auto etc_conf_reader(const std::string path) -> std::unordered_map<std::string, std::string>;

auto parse_interval_value(const std::string interval_string) -> std::chrono::seconds;

// Exposed for testing
auto parse_config_line(const std::string line) -> std::pair<std::string, std::string>;