// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#define FUSE_USE_VERSION 31

#include <iostream>

#include "log.hpp"
#include "fs.hpp"

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
    .write    = nullptr,
    .statfs   = nullptr,
    .release  = nullptr,
    .fsync    = nullptr,
    .readdir  = lake_readdir,
    .access   = nullptr,
    .create   = nullptr,
    .ioctl    = nullptr,
};

auto main() -> int {
    LOG("Initializing LakeFS");
    
    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);\

    const char* mount_point = "/lakefs";
    
    // run in foreground
    fuse_opt_add_arg(&args, "-f");
    
    // turn on debug mode
    fuse_opt_add_arg(&args, "-d");

    // mount point

    LOG("Mounting at " << mount_point);
    fuse_opt_add_arg(&args, mount_point);

    int ret = fuse_main(args.argc, args.argv, &operations, nullptr);

    fuse_opt_free_args(&args);

    return ret;
}