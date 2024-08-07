// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

import std.socket;

import src.command_interface;

int main(string[] args) {
    import std.stdio: writeln;
    import std.algorithm: any;

    // TODO: err 25 when this is done on the directory
    immutable string socket_path = "/tmp/lakefs.sock";

    Socket lakefs_socket = new Socket(AddressFamily.UNIX, SocketType.STREAM, ProtocolType.IP);
    auto lake_addr = new UnixAddress(socket_path);

    lakefs_socket.connect(lake_addr);

    if (any!"a == \"help\""(args)) {
        printHelp();

    } else if (any!"a == \"add\""(args)) {
        if (args.length < 3) {
            writeln("Error: add command requires a path argument");
            return 1;
        }

        return addFile(lakefs_socket, args[2]);

    } else if (any!"a == \"tag\""(args)) {
        if (args.length < 4) {
            writeln("Error: tag command requires a path and tag argument");
            return 1;
        }

        return tagFile(lakefs_socket, args[2], args[3]);

    } else { // if no command is given, print help
        printHelp();
    }

    return 0;
}

int addFile(ref Socket lake_s, string path) {
    import std.stdio: writeln;

    auto absolute_path = getAbsolutePath(path);

    writeln("Adding file " ~ absolute_path);

    auto data = new char[absolute_path.length + 1 + (int.sizeof*2)];;

    serialize_data(data.ptr, _LAKE_ADD_FILE, cast(uint) absolute_path.length, toCString(absolute_path).ptr);

    lake_s.send(data);

    return 0;
}

int tagFile(ref Socket lake_s, string path, string tag) {
    import std.stdio: writeln;

    auto absolute_path = getAbsolutePath(path);

    writeln("Tagging file " ~ absolute_path ~ " with tag " ~ tag);

    auto cmd_string = absolute_path ~ "\n" ~ tag;

    auto data = new char[cmd_string.length + 1 + (int.sizeof*2)];;

    serialize_data(data.ptr, _LAKE_TAG_FILE, cast(uint) cmd_string.length, toCString(cmd_string).ptr);

    lake_s.send(data);

    return 0;
}

void printHelp() {
    import std.stdio: writeln;

    writeln("lakefs-cli - A CLI toolkit for lakefs");
    writeln("Usage:");
    writeln("  lakefs-cli [command] [...arguments]");
    writeln("");
    writeln("Commands:");
    writeln("  add <path>       - Add a file to the lakefs");
    writeln("  tag <path> <tag> - Tag a file in the lakefs");
    writeln("  help             - Print this help message");
}

string getAbsolutePath(string path) @safe {
    import std.file: getcwd;

    return getcwd() ~ "/" ~ path; // TOD we will probably want to do full path resolution here
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