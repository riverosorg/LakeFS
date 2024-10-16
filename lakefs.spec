# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

Name:       LakeFS
Version:    0.0.1
Release:    1
Summary:    A tag based fs overlay
License:    BSD-3-Clause

BuildRequires:
Requires:

%description
Provides filesystem querying based on 'tags'.

%prep
make setup

%build
cat > hello-world.sh <<EOF
#!/usr/bin/bash
echo Hello world
EOF

%install
make install

%files
# DFffd

%changelog
# let's skip this for now