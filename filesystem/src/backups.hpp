// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <chrono>
#include <string>
#include <cstdint>

auto create_backup_timer(std::chrono::seconds interval, uint32_t number_backups, std::string backuppath) -> void;