// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

// Provides the filesystem interface through FUSE

extern "C" {
#include <fuse.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
}

#include "log.hpp"
#include "fs.hpp"

// Gets file attributes at <path>
int lake_getattr(const char *path, struct stat *stbuf) {

    LOG("Getting attributes for " << path);

    stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime = time( NULL );

    // Hard coding setting to directory
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;

    return 0;
}

int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {

    LOG("Reading directory " << path);
    
    // Check if path is a query
    // Check if path exists (This will be parsed by the query engine)

    // Return items in dir
    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);

    return 0;
}

int lake_open(const char *path, struct fuse_file_info *fi) {

    LOG("Opening file " << path);

    // Check if file exists

    // Determine if file is readable
 
    if ((fi->flags & O_ACCMODE) != O_RDONLY)
            return -EACCES;
    
    return 0;
}

int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {

    LOG("Reading file " << path);

    return 0;
}