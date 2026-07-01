# splash_thing

A [Hyprland](https://hyprland.org) plugin for managing the splash text shown on your monitor wallpaper.

## Features

- Loads custom splashes from `~/.config/hypr/splashes.conf` (one per line)
- Picks a random splash on startup
- Provides dispatchers to control splashes at runtime

## Build

```bash
make          # build release
make debug    # build debug
```

Requires `hyprland` and `lua` pkg-config modules.

## Install

```bash
make install  # copies to ~/.config/hypr/plugins/
```

Then load in your Hyprland config:

```
plugin = /path/to/splash_thing.so
```

## Usage

Create `~/.config/hypr/splashes.conf` with one splash per line:

```
hello there
welcome to hyprland
have a nice day
```

### Dispatchers

The plugin registers these dispatchers:

| Dispatcher              | Lua function                            | Description                                  |
| ----------------------- | --------------------------------------- | -------------------------------------------- |
| `setsplash <text>`      | `hl.plugin.splash_thing.set(text)`      | Set the current splash to `<text>`           |
| `addsplash <text>`      | `hl.plugin.splash_thing.add(text)`      | Add `<text>` to the splash list              |
| `removesplash <prefix>` | `hl.plugin.splash_thing.remove(prefix)` | Remove all splashes starting with `<prefix>` |
| `randomsplash`          | `hl.plugin.splash_thing.random()`       | Pick a random splash from the list           |

Usage with `hyprctl`:

```
hyprctl dispatch 'hl.plugin.splash_thing.set("hello")'
hyprctl dispatch 'hl.plugin.splash_thing.add("another splash")'
hyprctl dispatch 'hl.plugin.splash_thing.remove("prefix")'
hyprctl dispatch 'hl.plugin.splash_thing.random()'
```

Example keybinds (Lua config):

```lua
hl.bind("SUPER + S", function() hl.plugin.splash_thing.random() end)
hl.bind("SUPER + T", function() hl.plugin.splash_thing.set("hello") end)
```

## License

GPL-3.0
