// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#define FUSE_USE_VERSION 31

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>

#include "sqlite/sqlite3.h"

#include "db.hpp"
#include "fs.hpp"
#include "control.hpp"

static const struct fuse_operations operations = {
    .getattr  = lake_getattr,
    .readlink = nullptr,
    .mknod    = nullptr,
    .mkdir    = nullptr,
    .unlink   = nullptr,
    .rmdir    = nullptr,
    .symlink  = nullptr,
    .rename   = nullptr,
    .link     = nullptr,
    .chmod    = nullptr,
    .chown    = nullptr,
    .truncate = nullptr,
    .open     = lake_open,
    .read     = lake_read,
    .write    = lake_write,
    .statfs   = nullptr,
    .release  = lake_release,
    .fsync    = nullptr,
    .readdir  = lake_readdir,
    .access   = nullptr,
    .create   = nullptr,
    .ioctl    = nullptr,
};

auto main(char** argv, int argc) -> int {
    spdlog::set_level(spdlog::level::trace);

    spdlog::trace("Initializing LakeFS");
    
    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);

    const char* mount_point = "/lakefs";
    
    // run in foreground
    fuse_opt_add_arg(&args, "-f");

    // turn on debug mode
    fuse_opt_add_arg(&args, "-d");

    // so its usable
    // fuse_opt_add_arg(&args, "-oallow_other");

    // mount point
    spdlog::info("Mounting at {0}", mount_point);
    fuse_opt_add_arg(&args, mount_point);

    // Initialize SQLLite
    int rc = db_init();

    if (rc != SQLITE_OK) {
        spdlog::critical("Failed to initialize SQLite3");
        return 1;
    }

    // Initialize the control server
    std::thread control_thread(control_server);

    int ret = fuse_main(args.argc, args.argv, &operations, nullptr);

    fuse_opt_free_args(&args);

    return ret;
}