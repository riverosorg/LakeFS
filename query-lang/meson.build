# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

cpp = meson.get_compiler('cpp')

sources = [
    'src/ast.cpp',
]

depends = []


tests_dict = {
    'parsing' : ['tests/parsing.cpp', sources]
}

foreach name, src : tests_dict
  exe = executable(name, src, dependencies : depends, include_directories : ['src/'])
  test(name, exe)
endforeach
