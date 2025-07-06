# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

project(
    'LakeFS', 
    ['cpp', 'c', 'd'],
    version: '0.2.0',
    license: 'BSD-3-Clause'
)

add_project_arguments(['-w', '-std=c++23'], language : 'cpp')

add_project_arguments(['-w'], language : 'd')

# loading in individual subprojects

subdir('filesystem')
subdir('cli')

# The I testing should be loaded last to ensure that all other subprojects are built first
subdir('integration_tests')
