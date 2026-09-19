{
  description = "Rebang.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      buildPkgs = [
        pkgs.gnumake
        pkgs.patch
        pkgs.python3
        pkgs.ruff
        pkgs.ty
        pkgs.wineWow64Packages.stable
        pkgs.llvmPackages_21.clang-tools
      ];
      devPkgs = [
        pkgs.llvmPackages_21.bintools
        pkgs.radare2
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
