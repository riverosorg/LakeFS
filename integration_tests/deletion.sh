#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

touch $test_dir/test_file2_1
$cli add $test_dir/test_file2_1
$cli tag $test_dir/test_file2_1 default
$cli tag $test_dir/test_file2_1 not_default

# check file was added
results=$(ls $lake_dir | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: adding file not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# Remove a tag
$cli del-tag $test_dir/test_file2_1 not_default

results=$(ls -A $lake_dir/'(not_default)' | wc -l)

if [ $(echo "$results" | xargs) != "0" ]; then
    echo "Error: removing tag not working"
    echo "Expected: 0"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# Remove a file
$cli del $test_dir/test_file2_1

results=$(ls $lake_dir | wc -l)

if [ $(echo "$results" | xargs) != "0" ]; then
    echo "Error: removing file not working"
    echo "Expected: 0"
    echo "Got: $results"

    cleanup_and_exit 1
fi

cleanup_and_exit 0