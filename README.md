# Hyprland splash thing.
To use your own splashes or w/e.

## Build:
```
git clone https://github.com/ItsOhen/splash_thing.git
cd splash_thing
make release && make install
```
This builds it, and copies it over to ~/.config/hypr/plugins/splash_thing.so
From there, just load it with:
`hyprctl plugin load ~/.config/hypr/plugins/splash_thing.so`
Or you can just move it and load it however you want. It's all up to you!

## Usage:
1: Create a file named splashes.conf in your hyprland config directory.

2: Add the splashes you want, 1 per line.

3: Profit?


## Dispatchers
* setsplash \<text>

* addsplash \<text>

* removesplash \<text> ( will remove any splash starting with text )

* randomsplash

## Example:
`hyprctl dispatch setsplash "I FUCKING LOVE HYPRLAND!"`
