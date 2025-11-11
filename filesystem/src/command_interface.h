// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    // Command Interface

#define LAKE_SOCKET_PATH "/tmp/lakefs.sock"

#define LAKE_ADD_FILE 0
#define LAKE_TAG_FILE 1
#define LAKE_SET_DEFAULT_QUERY 2
#define LAKE_REMOVE_FILE 3
#define LAKE_REMOVE_TAG 4
#define LAKE_RELINK_FILE 5

    typedef struct __attribute__((packed)) lake_command_t
    {
        int command;
        int size;
        char data[];
    } lake_command_t;

#ifdef __cplusplus
}
#endif
