// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "config.hpp"

#include <spdlog/spdlog.h>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <optional>

auto etc_conf_reader(const std::string path) -> std::unordered_map<std::string, std::string> {
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

auto parse_config_line(const std::string line) -> std::pair<std::string, std::string> {
    std::string key;
    std::string value;

    std::istringstream line_stream(line);
    std::getline(line_stream, key, '=');
    std::getline(line_stream, value);

    return std::make_pair(key, value);
}

auto parse_interval_value(const std::string interval_string) -> std::optional<std::chrono::seconds> {
    using namespace std::chrono;

    std::optional<seconds> interval;

    const auto space_loc = interval_string.find_first_of(" ");
    const auto interval_length = std::stoi(interval_string.substr(0, space_loc));
    const auto interval_type = interval_string.substr(space_loc+1);

    if (interval_type == "days") {
        auto day_interval = days(interval_length);
        interval = duration_cast<seconds>(day_interval);

    } else if (interval_type == "hours") {
        auto hour_interval = hours(interval_length);
        interval = duration_cast<seconds>(hour_interval);

    } else if (interval_type == "weeks") {
        auto week_interval = weeks(interval_length);
        interval = duration_cast<seconds>(week_interval);

    } else if (interval_type == "months") {
        auto month_interval = months(interval_length);
        interval = duration_cast<seconds>(month_interval);

    } else {
        spdlog::critical("Unknown interval type: {0}", interval_type);
    }

    return interval;
}  