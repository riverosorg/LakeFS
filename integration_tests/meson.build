# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

tests_dict = {
    'test_cli': 'test_cli_control.sh',
}

foreach name, src : tests_dict
    exe = find_program(src)

    test(name, exe)
endforeach
