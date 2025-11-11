#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

mkdir $test_dir/tagged_dir
touch $test_dir/tagged_dir/makefile
mkdir $test_dir/tagged_dir/src
touch $test_dir/tagged_dir/src/a.c
touch $test_dir/tagged_dir/src/b.c
touch $test_dir/tagged_dir/src/c.c

$cli add $test_dir/tagged_dir
$cli tag $test_dir/tagged_dir its_a_folder

# Reading from a folder
results="$(ls -A $lake_dir/'(its_a_folder)'/tagged_dir/ | wc -l)"

if [ $(echo "$results" | xargs) != "2" ]; then
    echo "Error: Tagging a folder not working"
    echo "Expected: 2"
    echo "Got: $results"
    echo "ls: $(ls -A $lake_dir/'(its_a_folder)'/tagged_dir/)"

    cleanup_and_exit 1
fi

# Nested folder reading
results="$(ls -A $lake_dir/'(its_a_folder)'/tagged_dir/src/ | wc -l)"

if [ $(echo "$results" | xargs) != "3" ]; then
    echo "Error: Reading a nested folder not working!"
    echo "Expected: 3"
    echo "Got: $results"
    echo "ls: $(ls -A $lake_dir/'(its_a_folder)'/tagged_dir/src/)"

    cleanup_and_exit 1
fi

cleanup_and_exit 0