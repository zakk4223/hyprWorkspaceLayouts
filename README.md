# Workspace and monitor specific window layouts for Hyprland 


Do you wish you could run different window tiling layouts on individual workspaces?

## Using hyprpm, Hyprland's official plugin manager (recommended)
1. Run `hyprpm add https://github.com/zakk4223/hyprWorkspaceLayouts` and wait for hyprpm to build the plugin.
2. Run `hyprpm enable hyprWorkspaceLayouts`


## Using [hyprload](https://github.com/Duckonaut/hyprload)
Add the line `"zakk4223/hyprWorkspaceLayouts",` to your `hyprload.toml` config, like this

```toml
plugins = [
    "zakk4223/hyprWorkspaceLayouts",
]
```

Then update via the `hyprload,update` dispatcher.

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
