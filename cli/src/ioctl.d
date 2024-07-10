// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

module src.ioctl;

extern(C) extern __gshared immutable int FILE_FLAGS;

// We don't actually want to access these directly
extern(C) extern __gshared immutable int _LAKE_ADD_FILE;
extern(C) extern __gshared immutable int _LAKE_TAG_FILE;

extern(C) int ioctl(int fd, ulong request, ...);
extern(C) int open(const char* pathname, int flags, ...);
extern(C) int close(int fd);

// We have to pick up the FD to the lakefs

int addFile(int fd, string path) @trusted {
    string c_path = toCString(path);

    int test = 4;

    return ioctl(fd, _LAKE_ADD_FILE, test);
}

int tagFile(int fd, string path, string tag) @trusted {
    string c_path = toCString(path);
    string c_tag = toCString(tag);

    return ioctl(fd, _LAKE_TAG_FILE, &c_path[0], &c_tag[0]);
}

string toCString(string str) pure @safe {
    char[] cstr = new char[str.length + 1];
    
    foreach (i, c; str) {
        cstr[i] = c;
    }

    cstr[$-1] = '\0';
    
    return cstr;
}

unittest {
    assert(toCString("Hello, World!") == "Hello, World!\0");
}