#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

# Arbitrary tag query
echo "test2" >> $test_dir/test_file2

$cli tag $test_dir/test_file2 not_default

results=$(ls -A $lake_dir/'(not_default)' | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: Arbitrarily querying tags not working"
    echo "Expected: 1"
    echo "Got: $results"
    echo "Dir: $(ls -A $lake_dir/'(not_default)')"

    cleanup_and_exit 1
fi

# And tag search
$cli tag $test_dir/test_file2 tag2

results=$(ls -A $lake_dir/'(not_default&tag2)' | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: Querying tags with & not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# Test "or" operation
echo "test2" >> $test_dir/test_file1

$cli tag $test_dir/test_file1 tag1

results=$(ls -A $lake_dir/'(tag1|tag2)' | wc -l)

if [ $(echo "$results" | xargs) != "2" ]; then
    echo "Error: Querying tags with | not working"
    echo "Expected: 2"
    echo "Got: $results"
    echo "Dir: $(ls -A $lake_dir/'(tag1|tag2)')"

    cleanup_and_exit 1
fi

# Set a new default query
$cli default "(not_default)"

results=$(ls -A $lake_dir | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: changing default query not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# Check that a file with 2 tags will not show up when one is negated (Issue #15)
results=$(ls -A $lake_dir/'(!not_default)' | wc -l)

if [ $(echo "$results" | xargs) != "1" ]; then
    echo "Error: tag negation not working"
    echo "Expected: 1"
    echo "Got: $results"

    cleanup_and_exit 1
fi

cleanup_and_exit 0