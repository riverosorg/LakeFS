#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

touch $test_dir/test_file
$cli add $test_dir/test_file
$cli tag $test_dir/test_file default

mv $test_dir/test_file $test_dir/test_file2_1
$cli relink $test_dir/test_file $test_dir/test_file2_1

results="$(ls -A $lake_dir | grep test_file2_1 | wc -l)"

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: relinking not working"
    echo "Expected: 1"
    echo "Got: $results"
    echo "Extra info: $(ls -A $lake_dir)"

    cleanup_and_exit 1
fi

cleanup_and_exit 0