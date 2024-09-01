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
#include <spdlog/spdlog.h>

#include "ast.hpp"
#include "parser.hpp"
#include "fs.hpp"
#include "db.hpp"
#include "command_interface.h"

std::string reverse_query(const char* path);

// Gets file attributes at <path>
int lake_getattr(const char *path, struct stat *stbuf) {

    spdlog::trace("Getting attributes for {0}", path);

    if (strcmp(path, "/" ) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
        return 0;
	}

    auto file = reverse_query(path);

    if (file.empty())
        return -ENOENT;

    stat(file.c_str(), stbuf);

    return 0;
}

int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {

    spdlog::trace("Reading directory {0}", path);
    
    // Check if path is a query
    // Check if path exists (This will be parsed by the query engine)

    // Return items in dir
    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);

    auto files = db_run_query(parse("default"));

    for (const auto& file : files) {
        const std::string file_name = file.substr(file.find_last_of("/") + 1);
        
        spdlog::trace("Will show file {0} as {1}", file, file_name);
        
        filler(buf, file_name.c_str(), nullptr, 0);
    }

    return 0;
}

int lake_open(const char *path, struct fuse_file_info *fi) {

    spdlog::trace("Opening file {0}", path);

    // Check if file exists

    // Determine if file is readable
 
    // if ((fi->flags & O_ACCMODE) != O_RDONLY)
    //         return -EACCES;

    auto file_path = reverse_query(path);

    if (file_path.empty())
        return -ENOENT;

    spdlog::trace("Found file {0} to open", file_path);

    fi->fh = open(file_path.c_str(), 0, fi->flags & O_ACCMODE);
    
    return 0;
}

int lake_release(const char *path, struct fuse_file_info *fi) {
    
    spdlog::trace("Releasing file {0}", path);

    if (close(fi->fh) != 0)
        return -errno;

    return 0;
}

int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {

    spdlog::trace("Reading file {0}", path);

    // determine our host file via path

    ssize_t bytes_read = pread(fi->fh, buf, size, offset);

    if (bytes_read == -1)
        return -errno;

    return bytes_read;
}

int lake_write(const char *path, const char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi) {
    
    spdlog::trace("Writing to file {0}", path);

    // determine our host file via path

    ssize_t bytes_written = pwrite(fi->fh, buf, size, offset);

    if (bytes_written == -1)
        return -errno;

    return bytes_written;
}

int lake_destroy(void* private_data) {
    // Clean up the filesystem properly

    db_close();

    unlink(LAKE_SOCKET_PATH);

    return 0;
}

std::string reverse_query(const char* path) {
    auto path_s = std::string(path);

    // TODO:
    auto query_files = db_run_query(parse("default"));

    // Get the file path by comparing the file name to the query results
    std::string file_path;

    const std::string looking_for_file = 
        std::string(path_s).substr(std::string(path_s).find_last_of("/") + 1);

    for (const auto& query_file : query_files) {
        
        const std::string query_file_name = 
            query_file.substr(query_file.find_last_of("/") + 1);

        if (query_file_name == looking_for_file) {
            file_path = query_file;
            break;
        }
    }

    return file_path;
}