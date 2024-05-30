// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <iostream>

// Provide some logging helpers for debugging

#define LOG(msg) \
    std::cout << __FILE__ << " : " << __LINE__ << " : " << msg << std::endl;