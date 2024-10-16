# SPDX-FileCopyrightText: 2024 Caleb Depatie
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
        
        in {
          # Built via `nix build` and run via `nix run`

          packages.default = pkgs.stdenv.mkDerivation rec {
            pname   = "badgestl";
            version = "0.1.1";
            
            src = self;

            # postPatch = "ls -la";

            # sourceRoot = self;

            nativeBuildInputs = with pkgs; [
              gcc
              meson
              ninja
              reuse
              dmd
              docutils

              argparse
              fuse
              spdlo
            ];

            mesonBuildType = "release";

            enableParallelBuilding = true;

            meta = with pkgs.lib; {
              maintainers = [ "Caleb Depatie" "Conner Tenn" ];
            };
          };

          # Built via `nix build` and run via `nix run`
          # Building as a package not required

          # Entered with `nix develop`
          devShells.default = pkgs.mkShell {
            packages = with pkgs; [ 
              gcc
              meson
              ninja
              reuse
              dmd
              docutils

              argparse
              fuse
              spdlog
              fmt
            ];
          };
        }
      );
}