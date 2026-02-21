# Workspace and monitor specific window layouts for Hyprland 

> [!IMPORTANT]
> Starting with version `0.54.0`, Hyprland has inbuilt support for per-workspace layouts, making this plugin obsolete. For more info, have a look at the [example](https://wiki.hypr.land/Configuring/Uncommon-tips--tricks/#per-workspace-layouts) in the Hyprland wiki.

Do you wish you could run different window tiling layouts on individual workspaces? This plugin makes it possible!

## Using hyprpm, Hyprland's official plugin manager (recommended)
1. Run `hyprpm add https://github.com/zakk4223/hyprWorkspaceLayouts` and wait for hyprpm to build the plugin.
2. Run `hyprpm enable hyprWorkspaceLayouts`
3. Add `exec-once = hyprpm reload -n` to your `hyprland.conf` to make sure the plugins are loaded into Hyprland at boot.*`-n` will make hyprpm send a notification if anything goes wrong (e.g. update needed)*


## Using [hyprload](https://github.com/Duckonaut/hyprload)
Add the line `"zakk4223/hyprWorkspaceLayouts",` to your `hyprload.toml` config, like this

```toml
plugins = [
    "zakk4223/hyprWorkspaceLayouts",
]
```

Then update via the `hyprload,update` dispatcher.

## NixOS Installation

Add this flake to your inputs, note that using a mismatched/unsupported Hyprland release will fail to build or load the plugin:

```nix
inputs = {
    hyprland.url = "github:hyprwm/Hyprland"; # follows development branch of hyprland
    hyprWorkspaceLayouts = {
        url = "github:zakk4223/hyprWorkspaceLayouts";
        inputs.hyprland.follows = "hyprland"; # to make sure that the plugin is built for the correct version of hyprland
    };
};
```

Then, in your `home-manager` config, add the plugin:

```nix
wayland.windowManager.hyprland = {
    enable = true;
    plugins = [ inputs.hyprWorkspaceLayouts.packages.${pkgs.system}.default ];
};
```

## Manual installation

1. Build the plugin 
    - `make all`
2. Load the plugin however you load other Hyprland plugins.
    - `exec-once = hyprctl plugin load <ABSOLUTE PATH TO workspaceLayoutPlugin.so>`

## Configuration 

Set a default layout to use for workspaces you don't specifically configure:
```
plugin {
    wslayout {
        default_layout=master
    }
}
```
If you don't set one, "dwindle" is used.


Set your global layout to 'workspacelayout'
```
general {
    layout=workspacelayout
}
```

Configure workspace specific layouts

```
workspace=1,layoutopt:wslayout-layout:master #workspace id
workspace=name:badanime,layoutopt:wslayout-layout:dwindle #named workspace
workspace=special,layoutopt:wslayout-layout:dwindle #special
workspace=special:hiddenyaoi,layoutopt:wslayout-layout:nstack #named special
```
## Dispatcher
You can dynamically change the layout of the currently active workspace
via a dispatcher.

```
hyprctl dispatch layoutmsg setlayout dwindle
```

## Per monitor layouts
If you want to configure a layout for each monitor, consider using one of my other plugins: [hyprland-workspace-rules](https://github.com/zakk4223/hyprland-workspace-rules)
