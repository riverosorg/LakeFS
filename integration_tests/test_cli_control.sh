#!/usr/bin/bash

# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

fs=../build/filesystem/lakefs
fs_prog=$(basename $fs)

lake_dir=/lakefs

cli=../build/cli/lakefs-cli

test_dir=/tmp/lakefs_test

# start up the FS
fusermount -u $lake_dir
rm /tmp/lakefs.sock

sudo $fs > /dev/null 2>&1 &

sleep 0.2

# Create a test directory
mkdir -p $test_dir

# test adding a file
rng=$RANDOM

echo "$rng" > $test_dir/test_file

# TODO: current limitation, i need to pass the full path

$cli add $(pwd)/test_file
$cli tag $(pwd)/test_file default

dir=$(ls $lake_dir | grep test_file)

if [ "$dir" != "test_file" ]; then
    echo "Error: test_file not found in $lake_dir directory"
    exit 1
fi

# test reading the file
results=$(cat $lake_dir/test_file)

if [ "$rng" != "$results" ]; then
    echo "Error: file contents do not match"
    echo "Expected: $rng"
    echo "Got: $results"
    exit 1
fi

sudo killall -SIGINT $fs_prog

wait

fusermount -u $lake_dir

rm /tmp/lakefs.sock
rm -rf $test_dir
