#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

source integration_tests/test_core.sh

# Issue #34 :: Exit gracefully with an incorrect query
# TODO: Seems to work but I only get 0 in $rc.
# /usr/bin/env ls $lake_dir/'((default | not_default) & )'
# rc=$?

# if [ $rc != 1 ]; then
#     echo "Error: lakefs not responding correctly with faulty command"
#     echo "Expected: 1"
#     echo "Got: $rc"

#     cleanup_and_exit 1
# fi

# Setting up lake for complicated queries
touch $test_dir/photo_6789
$cli add $test_dir/photo_6789
$cli tag $test_dir/photo_6789 photo ottawa bsd_con me 2023

touch $test_dir/photo_6747
$cli add $test_dir/photo_6747
$cli tag $test_dir/photo_6747 photo ottawa bsd_con me jill 2022

touch $test_dir/photo_4587
$cli add $test_dir/photo_4587
$cli tag $test_dir/photo_4587 photo ottawa bsd_con jill 2022

touch $test_dir/photo_35789
$cli add $test_dir/photo_35789
$cli tag $test_dir/photo_35789 photo ottawa bsd_con me jill 2024

touch $test_dir/photo_9714
$cli add $test_dir/photo_9714
$cli tag $test_dir/photo_9714 photo ottawa me 2023

touch $test_dir/photo_9851
$cli add $test_dir/photo_9851
$cli tag $test_dir/photo_9851 photo toronto me jill 2024

touch $test_dir/docs_5679
$cli add $test_dir/docs_5679
$cli tag $test_dir/docs_5679 document taxes 2024

touch $test_dir/docs_5689
$cli add $test_dir/docs_5689
$cli tag $test_dir/docs_5689 document taxes 2023

touch $test_dir/docs_9741
$cli add $test_dir/docs_9741
$cli tag $test_dir/docs_9741 document invoice 2023

# Issue #34 :: NOT operator (!) should be able to be used without a subquery
# results="$(ls -A $lake_dir/'(document & !2024)' | wc -l)"

# if [ $(echo "$results" | xargs) != "2" ]; then
#     echo "Error: Negation without subquery not working"
#     echo "Expected: 2"
#     echo "Got: $results"

#     cleanup_and_exit 1
# fi

# Issue #36 :: A trailing & in a query does not result in the same output as leading
results="$(ls -A $lake_dir/'(jill & (!toronto))' | wc -l)"

if [ $(echo "$results" | xargs) != "3" ]; then
    echo "Error: Leading & not working"
    echo "Expected: 3"
    echo "Got: $results"

    cleanup_and_exit 1
fi

results="$(ls -A $lake_dir/'((!toronto) & jill)' | wc -l)"

if [ $(echo "$results" | xargs) != "3" ]; then
    echo "Error: Trailing & not working"
    echo "Expected: 3"
    echo "Got: $results"

    cleanup_and_exit 1
fi

# -- Just a number of more complicated queries to try to break things --

cleanup_and_exit 0