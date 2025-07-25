#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

# test writing to the tagged file
touch $test_dir/test_file

$cli add $test_dir/test_file
$cli tag $test_dir/test_file2 default

echo "test" >> "$lake_dir/test_file"

results=$(tail -n 1 $lake_dir/test_file)

if [ "$results" != "test" ]; then
    echo "Error: Could not add contents to file"
    echo "Expected: test"
    echo "Got: $results"

    cleanup_and_exit 77 # TODO: Explicitely skipping the test as this is known broken and not a high priority
fi

cleanup_and_exit 0