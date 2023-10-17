{
  inputs.nixpkgs.url = github:nixos/nixpkgs;
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      deps = with pkgs; [ pkg-config llvm libusb cmake ];
    in
    {
      name = "lgufbrightness";
      devShell."${system}" = with pkgs; mkShell {
        buildInputs = [ lldb ] ++ deps;
      };
      defaultPackage."${system}" = with pkgs; stdenv.mkDerivation {
        name = "lgufbrightness";
        version = "2023-04-19";
        buildInputs = deps;
        src = ./.;
        installPhase = ''
          install -m755 -D lgufbrightness $out/bin/lgufbrightness
        '';
      };
    };
}
