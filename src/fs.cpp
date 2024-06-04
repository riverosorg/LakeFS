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

const std::string test_path = "/main/iso/template/iso/FD13LIVE.iso";

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
 
    if ((fi->flags & O_ACCMODE) != O_RDONLY)
            return -EACCES;
    
    return 0;
}

int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {

    LOG("Reading file " << path);

    return 0;
}