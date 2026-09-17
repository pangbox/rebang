{
  description = "Rebang.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      devShells.${system} = {
        default = pkgs.mkShell {
          packages = with pkgs; [
            python3
            ruff
            ty
            gnumake
            patch
            wineWow64Packages.stable
            llvmPackages_21.clang-tools
          ];
        };

        lint = pkgs.mkShell {
          packages = with pkgs; [
            actionlint
            shellcheck
            zizmor
          ];
        };
      };
    };
}
