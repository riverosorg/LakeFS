# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

project(
    'LakeFS', 
    ['cpp', 'c'],
    version: '0.0.1',
    license: 'BSD-3-Clause'
)

add_project_arguments(['-w', '-std=c++23'], language : 'cpp')

cpp = meson.get_compiler('cpp')

sources = [
    'src/sqlite/sqlite3.c'
]

depends = []

depends += cpp.find_library('fuse')

executable(
    'lakefs',
    sources: ['src/main.cpp', sources],
    dependencies: depends,
    install: true
)

tests_dict = {
    'sqlite' : ['tests/vendors/sqlite.cpp', sources]
}

foreach name, src : tests_dict
  exe = executable(name, src, dependencies : depends, include_directories : ['src/'])
  test(name, exe)
endforeach