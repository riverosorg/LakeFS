// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "command_interface.h"

#include <string.h>

const int _LAKE_TAG_FILE = LAKE_TAG_FILE;
const int _LAKE_SET_DEFAULT_QUERY = LAKE_SET_DEFAULT_QUERY;
const int _LAKE_REMOVE_FILE = LAKE_REMOVE_FILE;
const int _LAKE_REMOVE_TAG = LAKE_REMOVE_TAG;
const int _LAKE_RELINK_FILE = LAKE_RELINK_FILE;

const char* _LAKE_SOCKET_PATH = LAKE_SOCKET_PATH;


void serialize_data(char* buffer, const int command, const unsigned int size, const char* data) 
{
    memcpy(buffer, &command, sizeof(int));
    memcpy(buffer + sizeof(int), &size, sizeof(int));
    memcpy(buffer + sizeof(int)*2, data, size);
}