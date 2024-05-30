// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

#define FUSE_USE_VERSION 31

extern "C" {
#include <fuse.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
}

#include <iostream>

// Gets file attributes at <path>
static int lake_getattr(const char *path, struct stat *stbuf) {

    stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime = time( NULL );

    // Hard coding setting to directory
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;

    return 0;
}

static int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {
    
    // Check if path is a query
    // Check if path exists (This will be parsed by the query engine)

    // Return items in dir
    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);

    return 0;
}

static int lake_open(const char *path, struct fuse_file_info *fi) {
    // Check if file exists

    // Determine if file is readable
 
    if ((fi->flags & O_ACCMODE) != O_RDONLY)
            return -EACCES;
    
    return 0;
}

static int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    return 0;
}

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
    std::cout << "Initializing LakeFS" << std::endl;
    
    // Fuse gets initiated like a program and needs its own args
    fuse_args args = FUSE_ARGS_INIT(0, nullptr);\

    // run in foreground
    fuse_opt_add_arg(&args, "-f");
    
    // turn on debug mode
    fuse_opt_add_arg(&args, "-d");

    // mount point
    fuse_opt_add_arg(&args, "/lakefs");

    int ret = fuse_main(args.argc, args.argv, &operations, nullptr);

    fuse_opt_free_args(&args);

    return ret;
}