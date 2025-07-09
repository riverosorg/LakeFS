# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

project(
    'LakeFS', 
    ['cpp', 'c', 'd'],
    version: '0.2.1',
    license: 'BSD-3-Clause',
    meson_version: '1.7.0'
)

add_project_arguments(['-w', '-std=c++23'], language : 'cpp')

add_project_arguments(['-w'], language : 'd')

# loading in individual subprojects

subdir('filesystem')
subdir('cli')

if get_option('documentation')
    subdir('docs')
endif

# The I testing should be loaded last to ensure that all other subprojects are built first
subdir('integration_tests')
