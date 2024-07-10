// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: BSD-3-Clause

// Converting the #define's into variables in a .o D can use

// I have not found a D interface to ioctl
#include <sys/ioctl.h>
#include <fcntl.h>

#include "../filesystem/src/ioctl.h"

const int FILE_FLAGS = O_RDONLY;

const int _LAKE_ADD_FILE = LAKE_ADD_FILE;
const int _LAKE_TAG_FILE = LAKE_TAG_FILE;