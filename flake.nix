{
  description = "A plugin to use different layouts on workspaces";

  inputs = {
    nixpkgs.follows = "hyprland/nixpkgs";
    systems.follows = "hyprland/systems";
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
      hyprWorkspaceLayouts = pkgsFor.${system}.callPackage ({
        lib,
        hyprlandPlugins,
      }:
        hyprlandPlugins.mkHyprlandPlugin inputs.hyprland.packages.${system}.hyprland {
          pluginName = "hyprWorkspaceLayouts";
          version = "0-unstable-2025-09-15";

          src = ./.;

          nativeBuildInputs = [pkgsFor.${system}.gnumake];

          buildPhase = ''
            runHook preBuild
            make all
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall
            mkdir -p $out/lib
            mv workspaceLayoutPlugin.so $out/lib/libhyprWorkspaceLayouts.so
            runHook postInstall
          '';

          meta = {
            homepage = "https://github.com/zakk4223/hyprWorkspaceLayouts";
            description = "Workspace-specific window layouts for Hyprland";
            license = lib.licenses.bsd3;
            platforms = lib.platforms.linux;
            maintainers = [];
          };
        }) {};
    });

    devShells = eachSystem (system: {
      default = pkgsFor.${system}.mkShell.override {stdenv = pkgsFor.${system}.gcc14Stdenv;} {
        name = "hyprWorkspaceLayouts";
        buildInputs = [inputs.hyprland.packages.${system}.hyprland];
        inputsFrom = [inputs.hyprland.packages.${system}.hyprland];
      };
    });
  };
}
