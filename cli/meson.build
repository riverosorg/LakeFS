# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

srcs = [
    'src/main.d',
    'src/ioctl.d',
    'src/ioctl.c',
]

executable('lakefs-cli', [srcs])

exe = executable('cli-unit-tests', srcs, d_unittest: true)
test('cli-unit-tests', exe)