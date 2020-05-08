# switch-cheats-updater
<p align="center">
<img src = "https://user-images.githubusercontent.com/61667930/78696328-916c2a00-78ff-11ea-95e4-f104e8428e64.jpg"\><br>
<img alt="GitHub All Releases" src="https://img.shields.io/github/downloads/HamletDuFromage/switch-cheats-updater/total">
</p>

A Nintendo Switch homebrew app to automatically update your cheat files for your installed games, gamecarts and XCIs. Supports Atmosphere and SXOS.


Fetches the latest cheats from the [gbatemp general thread](https://gbatemp.net/download/cheat-codes-sxos-and-ams-main-cheat-file-updated.36311/updates) through [my mirror](https://github.com/HamletDuFromage/switch-cheats-db/releases) (updates once a day)

## Blacklist titles
By default, this homebrew will overwrite the existing cheats. If you have your own cheat files that you'd like to keep as is, you may add the title IDs of the corresponding games, one ID per line, to `/config/cheats-updater/exclude.txt`. Use http://tinfoil.io/title to find the title ID of a game.

## How to install
Copy the `switch-cheats-updater/` directory to `/switch/` in your sdcard

## Screenshots
![done](https://user-images.githubusercontent.com/61667930/79614220-643e2980-8100-11ea-83ec-0a6002f1f6b5.jpg)

## Thanks
[ITotalJustice](https://github.com/ITotalJustice) for his download functions

The AtlasNX discord for helping me various with libnx questions in the past few weeks
