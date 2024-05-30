// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

extern "C" {
#include <fuse.h>
}

int lake_getattr(const char *path, struct stat *stbuf);

int lake_readdir(
    const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi);

int lake_open(const char *path, struct fuse_file_info *fi);

int lake_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi);