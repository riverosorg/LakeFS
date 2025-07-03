# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

Name:       LakeFS
Version:    0.1.0
Release:    1%{?dist}
Summary:    A tag based fs overlay
License:    BSD-3-Clause

BuildRequires: gcc dmd ninja meson argparse-devel spdlog-devel fmt-devel fuse3-devel
Requires: fuse3-libs

%description
Provides filesystem querying based on 'tags'.

%prep
nix develop
make setup

%build
make build

%install
make install

%check
make test

%files
/usr/bin/lakefs-cli /usr/bin/lakefs /etc/lakefs.conf

%changelog
#* 0.1.0 - Initial version