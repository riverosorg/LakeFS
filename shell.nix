# SPDX-FileCopyrightText: 2024 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# I want this to be a flake at some point, but the complexity seemed high
#note, shell presently not working to contain dependencies. 
# FUSE and sqlite3 must be seperately installed

{
  pkgs ? import <nixpkgs> {} 
}:  pkgs.mkShell {
  nativeBuildInputs = with pkgs.buildPackages; [
    gcc
    meson
    ninja
    reuse
    dmd
  ];

  buildInputs = with pkgs; [
    fuse
    spdlog
    fmt
  ];
}