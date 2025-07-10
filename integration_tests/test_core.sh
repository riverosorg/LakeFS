#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

#intended to be called via "source"

fs=../build/filesystem/lakefs
fs_prog=$(basename $fs)

lake_dir=./lakefs

cli=../build/cli/lakefs-ctl

test_dir=./lakefs_test

# Cleanup and exit with an error code
function cleanup_and_exit {
    if [ $(uname) = "FreeBSD" ]; then
        umount $lake_dir #fusermount not on freebsd, but definitely preferred
    else
        fusermount -u $lake_dir
    fi
    rm -rf $test_dir
    rm /tmp/lakefs.sock

    exit $1
}

# Create a needed test dirs
mkdir -p $test_dir
mkdir -p $lake_dir

# start up the FS
fusermount -u $lake_dir

$fs --tempdb $lake_dir &

sleep 0.2