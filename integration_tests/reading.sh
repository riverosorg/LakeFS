#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

rng=$RANDOM

echo "$rng" > $test_dir/test_file

$cli tag $test_dir/test_file default

# test reading the file
results="$(cat $lake_dir/test_file)"

if [ "$rng" != "$results" ]; then
    echo "Error: file contents do not match"
    echo "Expected: $rng"
    echo "Got: $results"

    cleanup_and_exit 1
fi

cleanup_and_exit 0
