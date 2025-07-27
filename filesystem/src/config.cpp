// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "config.hpp"

#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <utility>

auto etc_conf_reader(std::string path) -> std::unordered_map<std::string, std::string> {
    std::unordered_map<std::string, std::string> config;

    std::ifstream file(path);

    if (!file.is_open()) {
        spdlog::error("Failed to open {0}", path);
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '#') {
            continue;
        }

        auto key_value_pair = parse_config_line(line);

        config.insert(key_value_pair);

        spdlog::debug("Config line: {0}={1}", key_value_pair.first, key_value_pair.second);
    }

    return config;
}

auto parse_config_line(std::string line) -> std::pair<std::string, std::string> {
    std::string key;
    std::string value;

    std::istringstream line_stream(line);
    std::getline(line_stream, key, '=');
    std::getline(line_stream, value);

    return std::make_pair(key, value);
}