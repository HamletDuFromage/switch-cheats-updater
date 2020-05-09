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
![Home](https://user-images.githubusercontent.com/61667930/81458764-74e03d80-919c-11ea-87da-0073dad8aebe.jpg)
![Done](https://user-images.githubusercontent.com/61667930/81458769-81649600-919c-11ea-813d-6c34bc4266c7.jpg)

## Thanks
- [wilsam239 (Acta)](https://github.com/wilsam239) for his [significant QOL contribtions](https://github.com/HamletDuFromage/switch-cheats-updater/pull/3)

- [ITotalJustice](https://github.com/ITotalJustice) for his download functions

- The AtlasNX discord for helping me various with libnx questions in the past few weeks
