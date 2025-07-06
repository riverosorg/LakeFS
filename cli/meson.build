# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

srcs = [
    'src/main.d',
    'src/command_interface.d',
    'src/commands.d',
    'src/command_interface.c'
]

executable(
  'lakefs-ctl', 
  [srcs],
  include_directories: '../filesystem/src',
  install: true
)

# Leaving out as the unit tests dont currently /do/ anything
#test_srcs = {
#    'LakeFS-Ctl Main' : 'src/main.d'
#}
#
#foreach name, src : test_srcs
#  exe = executable(
#    name, 
#    [src, 'src/command_interface.d', 'src/command_interface.c'], 
#    include_directories: '../filesystem/src',
#    d_unittest: true
#  )
#  test(name, exe)
#endforeach