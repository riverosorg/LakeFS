# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

doc_files = [
    'lakefs-ctl',
    'lakefs.conf',
    'lakefs'
]

project_version = meson.project_version()

conf_data = configuration_data()
conf_data.set('version', project_version)

foreach file_name : doc_files
    configure_file(
        input : file_name + '.rst.in',
        output : file_name + '.rst',
        configuration : conf_data
    )

    env = environment()

    pwd = meson.current_build_dir() + '/'

    run_command('rst2man', pwd + file_name + '.rst', pwd + file_name + '.1', check: true, env: env)
    run_command('gzip', pwd + file_name + '.1', check: true, env: env)
endforeach