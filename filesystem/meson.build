# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

cpp = meson.get_compiler('cpp')

project_version = meson.project_version()

conf_data = configuration_data()
conf_data.set('version', project_version)

configure_file(
    input : 'src/metadata.h.in',
    output : 'metadata.h',
    configuration : conf_data
)

sources = [
    'src/fs.cpp',
    'src/db.cpp',
    'src/backups.cpp',
    'src/config.cpp',
    'src/control.cpp',
    'src/utilities.cpp',
    'src/query_lang/ast.cpp',
    'src/query_lang/parser.cpp'
]

add_project_arguments(['-DSPDLOG_FMT_EXTERNAL'], language : 'cpp')

depends = []

fuse_dep = cpp.find_library('fuse', required: false)
fuse3_dep = cpp.find_library('fuse3', required: false)

if not (fuse_dep.found() or fuse3_dep.found())
    error('Fuse dependency not found')
endif
depends += fuse3_dep
depends += fuse_dep

depends += cpp.find_library('spdlog')
depends += cpp.find_library('fmt')
depends += dependency('threads')

# Needed to add timer
add_project_link_arguments('-lrt', language : 'cpp')

includes = ['src/', '/usr/local/include']

if fuse3_dep.found()
    includes += '/usr/local/include/fuse3'
endif

sqlite3_proj = subproject('sqlite3')

depends += sqlite3_proj.get_variable('sqlite3_dep')

executable(
    'lakefs',
    sources: ['src/main.cpp', sources],
    dependencies: depends,
    include_directories: includes,
    install: true,
)

tests_dict = {
    'SQLite' : ['tests/vendors/sqlite.cpp', sources],
    'Utility Functions' : ['tests/utilities.cpp', sources],
    'Parsing' : ['tests/parsing.cpp', sources],
    'Config Reading' : ['tests/config.cpp', sources],
    'SQL generation' : ['tests/query_generation.cpp', sources],
}

catch2_proj = subproject('catch2')

depends += catch2_proj.get_variable('catch2_with_main_dep')

foreach name, src : tests_dict
    exe = executable(
        name, 
        src, 
        dependencies : depends, 
        include_directories : includes,
    )

    test(name, exe)
endforeach
