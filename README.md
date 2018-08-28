# BlackCrow
A simple StarCraft: Brood War bot that is built from scratch using [BWAPI](https://github.com/bwapi/bwapi).

This is a personal learning project for c++.

Features used:
- BWAPI 4.1.2
- Starcraft 1.16.1
- Visual Studio 2017
- .exe compiled output (opposed to being loaded as a dll)

## Status
Currently on hold until BWAPI 5 releases which will feature a language independent network bridge instead of a game hook. See [here](https://github.com/bwapi/bwapi/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen++5.x%3A).

## SSCAIT
You can [vote](https://sscaitournament.com/index.php?action=voteForPlayers&botId=443) the bot on the Twitch [Student StarCraft AI Tournament & Ladder Channel ](https://www.twitch.tv/sscait) and watch a game live.

## How to run
1. Install Starcraft 1.16.1
2. Install [Chaos Launcher](https://github.com/bwapi/bwapi/releases/tag/v4.1.2) from BWAPI with the BWAPI 4.1.2 Plugin
3. Download `BlackCrow.exe` and `BWAPI.dll` from the [v1.0 release page](https://github.com/baschermann/BlackCrow/releases/tag/v1.0) 
4. Put `BlackCrow.exe` and `BWAPI.dll` in the `bwapi-data` folder configured by the BWAPI 4.1.2 plugin config file
5. Start `BlackCrow.exe`, wait until repeated "Game table mapping not found." messages
6. Start StarCraft through the BWAPI 4.1.2 [RELEASE] plugin in the Chaos Launcher
7. Bot will take over control of the game and draw custom debug information on screen

## Results
Black Crow finished 19th of 78 participants in the [SSCAIT Tournament 2017/2018](https://sscaitournament.com/index.php?action=2017) with a win rate of 70.13%.
