# Workspace and monitor specific window layouts for Hyprland 


Do you wish you could run different window tiling layouts on individual workspaces? Or on specific monitors? Lucky you, that plugin lets you do that!

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


Configure workspace and monitor specific layouts:

```
workspace_layout=1,master #workspace id
workspace_layout=name:badanime,dwindle #named workspace
workspace_layout=special,dwindle #special
workspace_layout=special:hiddenyaoi,nstack #named special
```

```
monitor_layout=DP-1,master #monitor name
monitor_layout=desc:SDC 0x05234,dwindle #monitor description
```

## Dispatcher
You can dynamically change the layout of the currently active workspace
via a dispatcher.

```
hyprctl dispatch layoutmsg setlayout dwindle
```

