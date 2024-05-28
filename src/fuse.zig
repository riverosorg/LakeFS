// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

const std = @import("std");

pub export fn x(y: c_int) c_int {
    return y + 2;
}
