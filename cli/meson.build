# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

srcs = [
    'src/main.d',
    'src/command_interface.d',
    'src/command_interface.c'
]

executable(
  'lakefs-cli', 
  [srcs],
  include_directories: '../filesystem/src',
  install: true
)

test_srcs = {
    'LakeFS-CLI Main' : 'src/main.d'
}

foreach name, src : test_srcs
  exe = executable(
    name, 
    [src, 'src/command_interface.d', 'src/command_interface.c'], 
    include_directories: '../filesystem/src',
    d_unittest: true
  )
  test(name, exe)
endforeach