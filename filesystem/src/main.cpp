// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#define FUSE_USE_VERSION 31

#include <iostream>
#include <thread>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

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
    .destroy  = lake_destroy,
    .access   = nullptr,
    .create   = nullptr,
    .ioctl    = nullptr,
};

auto main(char** argv, int argc) -> int {
    // Initialize file logger
    // auto file_logger = spdlog::basic_logger_mt("file_logger", "lakefs.log");
    // spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::trace);

    spdlog::trace("Initializing LakeFS");
    
    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);

    const char* mount_point = "/lakefs";
    
    // run in foreground
    fuse_opt_add_arg(&args, "-f");
    fuse_opt_add_arg(&args, "-d");
    // TODO: We rely on foreground running for our control thread.
    // We may want to fork our own process and run in the background
    // Giving the caller behaviour expected of launching a

    // 
    fuse_opt_add_arg(&args, "-odefault_permissions");

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