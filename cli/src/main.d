// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

import std.socket;

import src.command_interface;
import src.commands;

int main(string[] args) {
    import std.stdio: writeln;
    import std.algorithm: any;
    import std.conv: to;

    if (any!"a == \"help\""(args)) {
        printHelp();

        return 0;
    }

    Socket lakefs_socket = new Socket(AddressFamily.UNIX, SocketType.STREAM, ProtocolType.IP);
    auto lake_addr = new UnixAddress(to!string(_LAKE_SOCKET_PATH));

    try {
        lakefs_socket.connect(lake_addr);
    } catch (Exception e) {
        writeln("Error: Could not connect to Lakefs database");
        writeln("Check if it is running and permissions are correct");

        return 1;
    }

    lakefs_socket.setOption(SocketOptionLevel.SOCKET, SocketOption.TCP_NODELAY, 1);

    scope(exit) lakefs_socket.close();

    // TODO: Cases can be done via metaprogramming
    if (any!"a == \"add\""(args)) {
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

        return tagFile(lakefs_socket, args[2], args[3 .. $]);
   
    } else if (any!"a == \"default\""(args)) {
        if (args.length < 3) {
            writeln("Error: default command requires a query argument");
            return 1;
        }

        return setDefault(lakefs_socket, args[2]);

    } else if (any!"a == \"del\""(args)) {
        if (args.length < 3) {
            writeln("Error: del command requires a file path argument");
            return 1;
        }

        return removeFile(lakefs_socket, args[2]);

    } else if (any!"a == \"del-tag\""(args)) {
        if (args.length < 4) {
            writeln("Error: del-tag command requires a file path and tag argument");
            return 1;
        }

        return removeTag(lakefs_socket, args[2], args[3]);

    } else if (any!"a == \"relink\""(args)) {
        if (args.length < 4) {
            writeln("Error: relink command requires an old and new file path");
            return 1;
        }

        return relink(lakefs_socket, args[2], args[3]);

    } else { // if no command is given, print help
        printHelp();

        return 1;
    }

    return 0;
}

void printHelp() {
    import std.stdio: writeln;

    writeln("lakefs-cli - A CLI toolkit for lakefs");
    writeln("Usage:");
    writeln("  lakefs-cli [command] [...arguments]");
    writeln("");
    writeln("Commands:");
    writeln("  help                          - Print this help message");
    writeln("  add     <path>                - Add a file to the lakefs");
    writeln("  tag     <path> <tag>          - Tag a file in the lakefs");
    writeln("  del     <path>                - Remove a file from the lakefs");
    writeln("  del-tag <path> <tag>          - Remove a tag from a file");
    writeln("  relink  <old path> <new path> - Transfer tags from an old file location to a new one");
    writeln("  default <query>               - Set the default query for the mounted directory");
}