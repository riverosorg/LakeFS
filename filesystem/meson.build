# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

cpp = meson.get_compiler('cpp')

sources = [
    'src/sqlite/sqlite3.c',
    'src/fs.cpp',
    'src/db.cpp',
    'src/control.cpp'
]

add_project_arguments(['-DSPDLOG_FMT_EXTERNAL'], language : 'cpp')

depends = []

depends += cpp.find_library('fuse')
depends += cpp.find_library('spdlog')
depends += cpp.find_library('fmt')

executable(
    'lakefs',
    sources: ['src/main.cpp', sources],
    dependencies: depends,
    install: true,
)

tests_dict = {
    'sqlite' : ['tests/vendors/sqlite.cpp', sources]
}

foreach name, src : tests_dict
  exe = executable(name, src, dependencies : depends, include_directories : ['src/'])
  test(name, exe)
endforeach