# SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# The flake allows locking dependencies. Given our use of the relatively quick changing
# Zig, having more control over when we upgrade is useful
{
  description = "LakeFS provides filesystem querying based on 'tags'.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem 
      (system:
        let 
          pkgs = nixpkgs.legacyPackages.${system}; 

          build_pkgs = with pkgs; [
            gcc
            meson
            ninja
            reuse
            dmd
            docutils

            argparse
            fuse
            spdlog
          ];

          pkg_meta = with pkgs.lib; {
            license = [ licenses.bsd3 ];
            maintainers = [ "Caleb Depatie" "Conner Tenn" ];
          };
        
        in {
          # Built via `nix build` and run via `nix run`
          packages.default = pkgs.stdenv.mkDerivation rec {
            pname   = "lakefs";
            version = "0.2.1";
            
            src = self;

            nativeBuildInputs = build_pkgs;

            mesonBuildType = "release";

            enableParallelBuilding = true;

            meta = pkg_meta;
          };

          packages.cli = pkgs.stdenv.mkDerivation rec {
            pname   = "lakefs-ctl";
            version = "0.2.1";
            
            src = self;

            nativeBuildInputs = build_pkgs;

            mesonBuildType = "release";

            enableParallelBuilding = true;

            meta = pkg_meta;
          };

          # Built via `nix build` and run via `nix run`
          # Building as a package not required

          # Entered with `nix develop`
          devShells.default = pkgs.mkShell {
            packages = build_pkgs;
          };
        }
      );
}