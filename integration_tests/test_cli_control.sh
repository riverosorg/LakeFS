#!/usr/bin/bash

# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

fs=../build/filesystem/lakefs
fs_prog=$(basename $fs)

lake_dir=./lakefs

cli=../build/cli/lakefs-cli

test_dir=./lakefs_test

# Cleanup and exit with an error code
function cleanup_and_exit {
    fusermount -u $lake_dir
    rm -rf $test_dir

    exit $1
}

# Create a needed test dirs
mkdir -p $test_dir
mkdir -p $lake_dir

# start up the FS
fusermount -u $lake_dir

sudo $fs $lake_dir &

sleep 0.2

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

# Arbitrary tag query
echo "test2" >> $test_dir/test_file2

$cli add $test_dir/test_file2
$cli tag $test_dir/test_file2 not_default

results=$(ls -A $lake_dir/'(not_default)' | wc -l)

if [ "$results" != "1" ]; then
    echo "Error: Arbitrarily querying tags not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# reading from query
results=$(cat $lake_dir/'(not_default)'/test_file2)

if [ "$results" != "test2" ]; then
    echo "Error: Reading from arbitrarily querying tags not working"
    echo "Expected: test2"
    echo "Got: $results"

    cleanup_and_exit 1
fi


# Set a new default query
$cli default "(not_default|default)"

results=$(ls -A $lake_dir | wc -l)

if [ "$results" != "2" ]; then
    echo "Error: changing default query not working"
    echo "Expected: 2"
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