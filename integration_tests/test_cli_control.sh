#!/usr/bin/bash

# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

fs=../build/filesystem/lakefs
fs_prog=$(basename $fs)

lake_dir=/lakefs

cli=../build/cli/lakefs-cli

test_dir=./lakefs_test

# Cleanup and exit with an error code
function cleanup_and_exit {
    fusermount -u $lake_dir
    rm -rf $test_dir

    exit $1
}

# start up the FS
fusermount -u $lake_dir

sudo $fs &

sleep 0.2

# Create a test directory
mkdir -p $test_dir

# test adding a file
rng=$RANDOM

echo "$rng" > $test_dir/test_file

# TODO: current limitation, i need to pass the relative path

$cli add $test_dir/test_file
$cli tag $test_dir/test_file default

dir=$(ls $lake_dir | grep test_file)

if [ "$dir" != "test_file" ]; then
    echo "Error: test_file not found in $lake_dir directory"

    cleanup_and_exit 1
fi

# test reading the file
results=$(cat $lake_dir/test_file)

if [ "$rng" != "$results" ]; then
    echo "Error: file contents do not match"
    echo "Expected: $rng"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# TODO: writing currently broken
# test writing to the tagged file
# echo "test" >> "$lake_dir/test_file"
# results=$(tail -n 1 $lake_dir/test_file)

# if [ "$results" != "test" ]; then
#     echo "Error: Could not add contents to file"
#     echo "Expected: test"
#     echo "Got: $results"

#     cleanup_and_exit 1
# fi

cleanup_and_exit 0