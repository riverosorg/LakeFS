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

#include <string>

#include "log.hpp"
#include "fs.hpp"
#include "db.hpp"
#include "command_interface.h"

const std::string test_path = "/testfile";

// Gets file attributes at <path>
int lake_getattr(const char *path, struct stat *stbuf) {

    LOG("Getting attributes for " << path);

    // This is going to be more complex. We'll need to effectively reverse search
    // whatever our query is to get the proper path from the name

    if (strcmp(path, "/" ) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
        return 0;
	}

    stat(test_path.c_str(), stbuf);

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

    auto files = db_tmp_query();

    for (const auto& file : files) {
        const std::string file_name = file.substr(file.find_last_of("/") + 1);
        
        LOG("Will show file " << file << " as " << file_name);
        
        filler(buf, file_name.c_str(), nullptr, 0);
    }

    return 0;
}

int lake_open(const char *path, struct fuse_file_info *fi) {

    LOG("Opening file " << path);

    // Check if file exists

    // Determine if file is readable
 
    // if ((fi->flags & O_ACCMODE) != O_RDONLY)
    //         return -EACCES;

    fi->fh = open(test_path.c_str(), 0, fi->flags & O_ACCMODE);
    
    return 0;
}

int lake_release(const char *path, struct fuse_file_info *fi) {
    
    LOG("Releasing file " << path);

    if (close(fi->fh) != 0)
        return -errno;

    return 0;
}

int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {

    LOG("Reading file " << path);

    // determine our host file via path

    ssize_t bytes_read = pread(fi->fh, buf, size, offset);

    if (bytes_read == -1)
        return -errno;

    return bytes_read;
}

int lake_write(const char *path, const char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi) {
    
    LOG("Writing to file " << path);

    // determine our host file via path

    ssize_t bytes_written = pwrite(fi->fh, buf, size, offset);

    if (bytes_written == -1)
        return -errno;

    return bytes_written;
}