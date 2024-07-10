// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

int main(string[] args) {
    import std.stdio: writeln;
    import std.algorithm: any;

    import src.ioctl;

    // TODO: err 25 when this is done on the directory
    immutable string mount_point = "/lakefs/testfile";

    const string c_mount_point = toCString(mount_point);

    const int lake_fd = open(c_mount_point.ptr, FILE_FLAGS);

    scope (exit) close(lake_fd);

    if (lake_fd == -1) {
        writeln("Error: Unable to open lakefs at ", mount_point);
        return 1;
    }

    if (any!"a == \"help\""(args)) {
        printHelp();

    } else if (any!"a == \"add\""(args)) {
        if (args.length < 3) {
            writeln("Error: add command requires a path argument");
            return 1;
        }

        return addFile(lake_fd, args[2]);

    } else if (any!"a == \"tag\""(args)) {
        if (args.length < 4) {
            writeln("Error: tag command requires a path and tag argument");
            return 1;
        }

        return tagFile(lake_fd, args[2], args[3]);

    } else { // if no command is given, print help
        printHelp();
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
    writeln("  add <path>       - Add a file to the lakefs");
    writeln("  tag <path> <tag> - Tag a file in the lakefs");
    writeln("  help             - Print this help message");
}