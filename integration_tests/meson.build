# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

tests_dict = {
    'Simple Queries': 'querying.sh',
    'Reading Files': 'reading.sh',
    'Writing Files': 'writing.sh',
    'Deleting Files': 'deletion.sh',
    'Relinking Files': 'relinking.sh',
    'Add multiple tags': 'multiple_tags.sh',
    'Tag Folders': 'folder.sh',
    'Advanced Queries': 'adv_query.sh'
}

configure_file(
    input: 'test_core.sh',
    output: 'test_core.sh',
    copy: true
)

foreach name, src : tests_dict
    exe = find_program(src)

    test(name, exe, 
        is_parallel: false, 
        suite: 'integration'
    )
endforeach
