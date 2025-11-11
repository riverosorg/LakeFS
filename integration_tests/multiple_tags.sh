#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

# Add multiple tags in one
echo "test" >> $test_dir/test_file

$cli tag $test_dir/test_file tag1 tag2

results="$(ls -A $lake_dir/'(tag1&tag2)' | wc -l)"

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: Tagging a file with 2 tags at once not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

cleanup_and_exit 0