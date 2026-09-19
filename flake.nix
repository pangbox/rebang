{
  description = "Rebang.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      python = pkgs.python3.withPackages (ps: [ ps.capstone ]);
      buildPkgs = [
        pkgs.gnumake
        pkgs.patch
        python
        pkgs.ruff
        pkgs.ty
        pkgs.wineWow64Packages.stable
        pkgs.llvmPackages_21.clang-tools
      ];
      devPkgs = [
        pkgs.llvmPackages_21.bintools
        pkgs.radare2
        pkgs.uv
      ];
      lintPkgs = [
        pkgs.actionlint
        pkgs.shellcheck
        pkgs.zizmor
      ];
    in {
      devShells.${system} = {
        default = pkgs.mkShell {
          packages = builtins.concatLists [
            buildPkgs
            devPkgs
            lintPkgs
          ];
        };
        build = pkgs.mkShell {
          packages = buildPkgs;
        };
        lint = pkgs.mkShell {
          packages = lintPkgs;
        };
      };
    };
}
