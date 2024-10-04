// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#include "command_interface.h"

#include <string.h>

const int _LAKE_ADD_FILE = LAKE_ADD_FILE;
const int _LAKE_TAG_FILE = LAKE_TAG_FILE;
const int _LAKE_SET_DEFAULT_QUERY = LAKE_SET_DEFAULT_QUERY;

void serialize_data(char* buffer, const int command, const unsigned int size, const char* data) 
{
    memcpy(buffer, &command, sizeof(int));
    memcpy(buffer + sizeof(int), &size, sizeof(int));
    memcpy(buffer + sizeof(int)*2, data, size);
}