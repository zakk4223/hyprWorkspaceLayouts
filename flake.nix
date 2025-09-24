{
  description = "A plugin to use different layouts on workspaces";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    hyprland.url = "github:hyprwm/Hyprland/v0.51.0";
  };

  outputs = {
    self,
    nixpkgs,
    ...
  } @ inputs: let
    systems = ["x86_64-linux"];
    eachSystem = nixpkgs.lib.genAttrs systems;
    pkgsFor = nixpkgs.legacyPackages;
  in {
    packages = eachSystem (system: {
      default = self.packages.${system}.hyprWorkspaceLayouts;
      hyprWorkspaceLayouts = let
        inherit (inputs.hyprland.packages.${system}) hyprland;
        inherit (pkgsFor.${system}) stdenvNoCC gcc14 lib;

        name = "hyprWorkspaceLayouts";
      in
        stdenvNoCC.mkDerivation {
          inherit name;
          pname = name;
          version = "0-unstable-2025-09-15";
          src = ./.;

          inherit (hyprland) buildInputs;
          nativeBuildInputs = hyprland.nativeBuildInputs ++ [hyprland gcc14];
          enableParallelBuilding = true;

          dontUseCmakeConfigure = true;
          dontUseMesonConfigure = true;
          dontUseNinjaBuild = true;
          dontUseNinjaInstall = true;

          installPhase = ''
            runHook preInstall

            mkdir -p "$out/lib"
            mv workspaceLayoutPlugin.so "$out/lib/lib${name}.so"

            runHook postInstall
          '';

          meta = {
            homepage = "https://github.com/zakk4223/hyprWorkspaceLayouts";
            description = "Workspace-specific window layouts for Hyprland";
            license = lib.licenses.bsd3;
            platforms = lib.platforms.linux;
            maintainers = [];
          };
        };
    });

    devShells = eachSystem (system: {
      default = pkgsFor.${system}.mkShell {
        name = "hyprWorkspaceLayouts";
        buildInputs = [inputs.hyprland.packages.${system}.hyprland];
        inputsFrom = [inputs.hyprland.packages.${system}.hyprland];
      };
    });
  };
}
