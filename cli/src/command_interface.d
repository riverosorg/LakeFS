// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

extern (C) extern __gshared immutable int _LAKE_ADD_FILE;
extern (C) extern __gshared immutable int _LAKE_TAG_FILE;
extern (C) extern __gshared immutable int _LAKE_SET_DEFAULT_QUERY;

extern (C) void serialize_data(char* dest, const int command, const uint size, const char* data);

// D's interface to C page implies there is no way to directly reference a C struct
// This is.. dissapointing and requires the below duplication of work,
// which will have to be kept in step with any changes to the C struct

struct lake_command_t {
    int command;
    int size;
    string data;
};