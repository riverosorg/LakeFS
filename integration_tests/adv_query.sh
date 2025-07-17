#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

# Issue #34 :: Exit gracefully with an incorrect query
results = $(ls $lakfs_dir/'(default & &)')

if [ $? != 1 ]; then
    echo "Error: lakefs not responding correctly with faulty command"
    echo "Expected: 1"
    echo "Got: $results"
    echo "Dir: $(ls -A $lake_dir/'(not_default)')"

    # cleanup_and_exit 1
fi

# Issue #34 :: NOT operator (!) should be able to be used without a subquery
echo "test" >> $test_dir/test_file
echo "test" >> $test_dir/test_file2

$cli add $test_dir/test_file
$cli add $test_dir/test_file not_default
$cli add $test_dir/test_file2
$cli add $test_dir/test_file2 not_default
$cli add $test_dir/test_file2 default

results = $(ls -A $lakfs_dir/'(not_default & !default)' | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: Negation without subquery not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi


cleanup_and_exit 0