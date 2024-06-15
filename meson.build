# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

project(
    'LakeFS', 
    ['cpp', 'c', 'd'],
    version: '0.0.1',
    license: 'BSD-3-Clause'
)

add_project_arguments(['-w', '-std=c++23'], language : 'cpp')

add_project_arguments(['-debug', '-w'], language : 'd')

# loading in individual subprojects

subdir('filesystem')
subdir('cli')