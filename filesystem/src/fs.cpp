// SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
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
#include <spdlog/spdlog.h>

#include "ast.hpp"
#include "parser.hpp"
#include "fs.hpp"
#include "db.hpp"
#include "control.hpp"
#include "command_interface.h"

std::string reverse_query(const char* path);
std::string extract_query(const char* path);

// Gets file attributes at <path>
#ifdef __FreeBSD__
int lake_getattr(const char *path, struct stat *stbuf, struct fuse_file_info* fi) {
#else
int lake_getattr(const char *path, struct stat *stbuf) {
#endif

    spdlog::trace("Getting attributes for {0}", path);

    if ((strcmp(path, "/") == 0) || (path[strlen(path) - 1] == ')'))
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

#ifdef __FreeBSD__
int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
#else
int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {
#endif 

    spdlog::trace("Reading directory {0}", path);
    

    // Return items in dir
#ifdef __FreeBSD__
    filler(buf, ".", nullptr, 0, FUSE_FILL_DIR_PLUS);
    filler(buf, "..", nullptr, 0, FUSE_FILL_DIR_PLUS);
#else
    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);
#endif

    std::vector<std::string> files;

    // Check if path is a query
    if (path[strlen(path) - 1] == ')') {
        std::string query = extract_query(path);

        try {
            files = db_run_query(parse(query));
        } catch (std::exception err) {
            spdlog::error("Error while parsing: {0}", err.what());
        }

    } else {
        files = db_run_default_query();
    }

    for (const auto& file : files) {
        const std::string file_name = file.substr(file.find_last_of("/") + 1);
        
        spdlog::trace("Will show file {0} as {1}", file, file_name);

#ifdef __FreeBSD__
        filler(buf, file_name.c_str(), nullptr, 0, FUSE_FILL_DIR_PLUS);
#else
        filler(buf, file_name.c_str(), nullptr, 0);
#endif
    }

    return 0;
}

int lake_open(const char *path, struct fuse_file_info *fi) {

    spdlog::trace("Opening file {0}", path);

    // Check if file exists

    // Determine if file is readable

    auto file_path = reverse_query(path);

    if (file_path.empty())
        return -ENOENT;

    spdlog::trace("Found file {0} to open", file_path);

    fi->fh = open(file_path.c_str(), 0, fi->flags & O_ACCMODE);
    
    return 0;
}

int lake_release(const char *path, struct fuse_file_info *fi) {
    
    spdlog::trace("Releasing file {0} at FD {1}", path, fi->fh);

    if (close(fi->fh) != 0)
        return -errno;

    fi->fh = -1;

    return 0;
}

int lake_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {

    spdlog::trace("Reading file {0} at fd {1}", path, fi->fh);

    // determine our host file via path

    ssize_t bytes_read = pread(fi->fh, buf, size, offset);

    if (bytes_read == -1)
        return -errno;

    return bytes_read;
}

int lake_write(const char *path, const char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi) {
    
    spdlog::trace("Writing to file {0} at fd {1}", path, fi->fh);

    // permissions issue? read works
    ssize_t bytes_written = pwrite(fi->fh, buf, size, offset);

    if (bytes_written == -1)
        return -errno;

    return bytes_written;
}

void lake_destroy(void* private_data) {
    // Clean up the filesystem properly

    spdlog::trace("Shutting down filesystem");

    int rc = db_close();

    if (rc != 0) {
        spdlog::error("Failed to close SQLite3 DB");
    }

    rc = unlink(LAKE_SOCKET_PATH);

    if (rc != 0) {
        spdlog::error("Failed to unlink socket");
    }
}

std::string reverse_query(const char* path) {
    auto path_s = std::string(path);

    std::vector<std::string> query_files;

    if (path_s.find_first_of('(') == std::string::npos) {
        query_files = db_run_default_query();

    } else {
        std::string query = extract_query(path);

        try {
            query_files = db_run_query(parse(query));
        
        } catch (std::exception err) {
            spdlog::error("Error while parsing: {0}", err.what());
            // todo: exit?
        }
    }

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

std::string extract_query(const char* path) {
    auto path_s = std::string(path);

    return path_s.substr(path_s.find_last_of('('), path_s.find_last_of(')') - path_s.find_last_of('('));
}