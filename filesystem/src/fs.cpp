// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

// Provides the filesystem interface through FUSE

extern "C" {
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
}

#include <string>
#include <cstdint>

#include "log.hpp"
#include "fs.hpp"
#include "db.hpp"
#include "ioctl.h"

const std::size_t MAX_PATH = 4096;

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

    // todo: we dont want to hardcode this
    {
        const std::string file_name = test_path.substr(test_path.find_last_of("/") + 1, test_path.size() - 1);
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

int lake_ioctl(const char *path, int cmd, void *arg,
            struct fuse_file_info *fi, unsigned int flags, void *data) {

    LOG("IOCTL " << cmd << " on file " << path);

    // This primarily functions to provide a controlling interface for the db

    if (flags & FUSE_IOCTL_COMPAT)
        return -ENOSYS;

    int result = 0;

    switch (cmd) {

        case LAKE_ADD_FILE: {
            // struct iovec iov = { arg, 4096 };
            // fuse_reply_ioctl_retry();

            LOG("T " << arg << " " << data);
            LOG(" aa " << *(int*)arg);

            const char* c_file_path = (const char *)arg;
            std::string file_path(c_file_path);

            LOG("Adding file " << file_path);

            // Add the file to the db
            result = db_add_file(file_path);

            break;
        }

        case LAKE_TAG_FILE: {
            
            break;
        }
    
        default: {
            LOG("Unknown IOCTL command " << cmd);

            result = -ENOSYS;
        }
    }

    return result;
}