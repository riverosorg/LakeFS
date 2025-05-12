# SPDX-FileCopyrightText: 2024 Caleb Depatie
# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

cpp = meson.get_compiler('cpp')

project_version = meson.project_version()

conf_data = configuration_data()
conf_data.set('version', project_version)

configure_file(
    input : 'src/config.h.in',
    output : 'config.h',
    configuration : conf_data
)

sources = [
    'src/sqlite/sqlite3.c',
    'src/fs.cpp',
    'src/db.cpp',
    'src/control.cpp',
    'src/ast.cpp',
    'src/parser.cpp'
]

add_project_arguments(['-DSPDLOG_FMT_EXTERNAL'], language : 'cpp')

depends = []

depends += cpp.find_library('fuse3')
depends += cpp.find_library('spdlog')
depends += cpp.find_library('fmt')
depends += dependency('threads')

includes = ['src/', '/usr/local/include', '/usr/local/include/fuse3']

executable(
    'lakefs',
    sources: ['src/main.cpp', sources],
    dependencies: depends,
    include_directories: includes,
    install: true,
)

tests_dict = {
    'sqlite' : ['tests/vendors/sqlite.cpp', sources],
    'parsing' : ['tests/parsing.cpp', sources],
    'SQL generation' : ['tests/query_generation.cpp', sources],
}

foreach name, src : tests_dict
    exe = executable(
        name, 
        src, 
        dependencies : depends, 
        include_directories : includes,
    )

    test(name, exe)
endforeach
