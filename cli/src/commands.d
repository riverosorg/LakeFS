// SPDX-FileCopyrightText: 2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

import std.socket;
import src.command_interface;

int sendMessage(ref Socket lake_s, const int command, string[] str_list...)
{
    import std.algorithm: fold;

    auto full_command_str = fold!((a, b) => a ~ "\n" ~ b)(str_list);

    auto data = new char[full_command_str.length + 1 + (int.sizeof*2)];

    serialize_data(data.ptr, command, cast(uint) full_command_str.length, toCString(full_command_str).ptr);

    lake_s.send(data);  

    return 0;
}

int tagFile(ref Socket lake_s, string path, string[] tags) {
    import core.thread.osthread: Thread;
    import core.time: dur;

    auto absolute_path = getAbsolutePath(path);

    int rc = 0;

    foreach(tag; tags) {
        debug {
            import std.stdio: writeln;
            writeln("Tagging file " ~ absolute_path ~ " with tag " ~ tag);
        }

        // TODO: actually handle RC. mostly ignored and difficult to be zero anyways rn
        rc = sendMessage(lake_s, _LAKE_TAG_FILE, absolute_path, tag);

        Thread.sleep(dur!("msecs")(10));
    }

    return rc;
}

int removeFile(ref Socket lake_s, string path) {

    auto absolute_path = getAbsolutePath(path);

    debug {
        import std.stdio: writeln;
        writeln("Removing file " ~ absolute_path);
    }

    return sendMessage(lake_s, _LAKE_REMOVE_FILE, absolute_path);
}

int removeTag(ref Socket lake_s, string path, string tag) {
    import std.stdio: writeln;

    auto absolute_path = getAbsolutePath(path);

    debug {
        import std.stdio: writeln;
        writeln("Removing tag " ~ tag ~ " from file " ~ absolute_path);
    }

    return sendMessage(lake_s, _LAKE_REMOVE_TAG, absolute_path, tag);
}

int relink(ref Socket lake_s, string old_path, string new_path) {
    
    auto absolute_path = getAbsolutePath(old_path);
    auto absolute_new_path = getAbsolutePath(new_path);

    debug {
        import std.stdio: writeln;
        writeln("Moving " ~ absolute_path ~ " from " ~  absolute_new_path);
    }

    return sendMessage(lake_s, _LAKE_RELINK_FILE, absolute_path, absolute_new_path);
}

int setDefault(ref Socket lake_s, string query) {
    
    debug {
        import std.stdio: writeln;
        writeln("Setting default query to " ~ query);
    }

    return sendMessage(lake_s, _LAKE_SET_DEFAULT_QUERY, query);
}

string getAbsolutePath(string path) @safe {
    import std.file: getcwd;

    if (path[0] == '/') {
        return path;
    } else {
        return getcwd() ~ "/" ~ path;
    } 
}

string toCString(string str) pure nothrow @safe {
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