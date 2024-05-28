# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# I want this to be a flake at some point, but the complexity seemed high

{
  pkgs ? import <nixpkgs> {} 
}:  pkgs.mkShell {
  nativeBuildInputs = with pkgs.buildPackages; [
    zig
    go
    sqlite
    fuse
    reuse
  ];
}