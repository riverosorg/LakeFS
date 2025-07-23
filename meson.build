# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
# SPDX-FileCopyrightText: 2024 Conner Tenn
#
# SPDX-License-Identifier: 0BSD

project(
    'LakeFS', 
    ['rust', 'c', 'd'],
    version: '0.3.0',
    license: 'BSD-3-Clause'
)

add_project_arguments(['-w'], language : 'd')

# loading in individual subprojects

subdir('cli')

if get_option('documentation')
    subdir('docs')
endif

# The I testing should be loaded last to ensure that all other subprojects are built first
subdir('integration_tests')
