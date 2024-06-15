# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

srcs = [
    'src/main.d'
]

executable('lakefs-cli', [srcs])

test_srcs = {
    'main' : 'src/main.d'
}

foreach name, src : test_srcs
  exe = executable(name, src, d_unittest: true)
  test(name, exe)
endforeach