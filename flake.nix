{
  inputs.nixpkgs.url = github:nixos/nixpkgs;
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
      deps = with pkgs; [ pkgconfig llvm libusb cmake ];
    in
    {
      name = "lgultrafine";
      devShell."${system}" = with pkgs; mkShell {
        buildInputs = [ lldb ] ++ deps;
      };
      defaultPackage."${system}" = with pkgs; stdenv.mkDerivation {
        name = "lgufbrightness";
        version = "2023-04-19";
        buildInputs = deps;
        src = ./.;
        installPhase = ''
          mkdir -p $out/bin
          cp lguf_brightness $out/bin/lgufbrightness
        '';
      };
    };
}
