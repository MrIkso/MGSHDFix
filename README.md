# Metal Gear Solid Master Collection Fix
[MG1 / MG2 NexusMods Page](https://www.nexusmods.com/metalgearandmetalgear2mc/mods/9) | [MGS2 NexusMods Page](https://www.nexusmods.com/metalgearsolid2mc/mods/49) | [MGS3 NexusMods Page](https://www.nexusmods.com/metalgearsolid3mc/mods/139) | **GitHub Repo (You're already here!)**<br />

[![Github All Releases](https://img.shields.io/github/downloads/ShizCalev/MGSHDFix/total.svg)](https://github.com/ShizCalev/MGSHDFix/releases)<br />

This is a fix that adds custom resolutions, ultrawide support and much more to the Metal Gear Solid Master Collection.<br />

## Games Supported
- Metal Gear 1/2 (MSX)
- Metal Gear Solid 2
- Metal Gear Solid 3

## Metal Gear Solid 1 / Metal Gear 1/2 (NES)
- For Metal Gear Solid 1 and the Vol 1. Bonus Content (MG1/2 NES), using [MGSM2Fix](https://github.com/nuggslet/MGSM2Fix) is recommended.

## Features
- Custom resolution/ultrawide support.
- Experimental 16:9 HUD option that resizes HUD/movies (MGS2/MGS3).
- Borderless/windowed mode.
- Mouse cursor toggle.
- Mouse sensitivity adjustment (MGS3).
- Corrects gameplay/cutscene aspect ratio (MGS2/MGS3).
- Corrects window size on displays with High DPI scaling enabled. [PR #127](https://github.com/Lyall/MGSHDFix/pull/127)
- Corrects the monitor going to sleep during long cutscenes.
- Launcher skip (see ini to configure).
- Skip intro logos (MGS2/MGS3).
- Adjustable anisotropic filtering (MGS2/MGS3).
- Option to disable bilinear texture filtering, giving the games a pixel art/retro appearance. [PR #138](https://github.com/Lyall/MGSHDFix/pull/138)
- Increased texture size limits (MG1/MG2/MGS3).
- Fixed water surface rendering (MGS3). See [PR #71](https://github.com/Lyall/MGSHDFix/pull/71) for a breakdown of the issue.
- Fixed vector effects / line based rendering scaling (ie rain, lasers, bullet trails.) [PR #140](https://github.com/Lyall/MGSHDFix/pull/140)
- Fixed broken skybox initialization procs (MGS2). [PR#142](https://github.com/Lyall/MGSHDFix/pull/142)

## Installation
- Grab the latest release of MGSHDFix from [here.](https://github.com/Lyall/MGSHDFix/releases)
- Extract the contents of the release zip in to the the game folder.<br />(e.g. "**steamapps\common\MGS2**" or "**steamapps\common\MGS3**" for Steam).

### Steam Deck/Linux Additional Instructions
🚩**You do not need to do this if you are using Windows!**
- Open up the game properties of either MGS2/MGS3 in Steam and add `WINEDLLOVERRIDES="wininet,winhttp=n,b" %command%` to the launch options.

## Configuration
- See **MGSHDFix.ini** to adjust settings for the fix.

## Known Issues
Please report any issues you see.
This list will contain bugs which may or may not be fixed.

### MGS 2
- Strength of post-processing may be reduced at higher resolutions. ([#35](https://github.com/Lyall/MGSHDFix/issues/35))
- Various visual issues when using the experimental HUD fix. ([#41](https://github.com/Lyall/MGSHDFix/issues/41))

### MGS 3
- Strength of post-processing may be reduced at higher resolutions. ([#35](https://github.com/Lyall/MGSHDFix/issues/35))
- Various visual issues when using the experimental HUD fix. ([#41](https://github.com/Lyall/MGSHDFix/issues/41))

## Screenshots

| ![ezgif-3-82fd6eedda](https://github.com/Lyall/MGSHDFix/assets/695941/b01453c7-b4ee-4903-bd34-340371873ecb) |
|:--:|
![2131640_20250428235619_1](https://github.com/user-attachments/assets/0ea1f53b-3be6-459b-9a7a-686dd14cf4e6)
![2131640_20250428225953_1](https://github.com/user-attachments/assets/73c80569-dd37-4879-a6fb-97dbb46c2f9b)
![2131640_20250501025301_1](https://github.com/user-attachments/assets/3f5eb079-1b64-45a6-992f-e0b421f414ad)
![2131640_20250501025407_1](https://github.com/user-attachments/assets/c9337d0e-16fa-47e8-beba-efc067e98acb)
| Metal Gear Solid 2 |

| ![ezgif-3-982e93f49a](https://github.com/Lyall/MGSHDFix/assets/695941/5530a42e-6b6a-4eb0-a714-ba3e7c3a1dc3) |
|:--:|
![2131650_20250429005837_1](https://github.com/user-attachments/assets/01b4258a-e6f3-4b24-8609-c1d21834426b)
![vlcsnap-2025-04-30-01h48m54s182](https://github.com/user-attachments/assets/aba7a75d-f114-4c7b-b41a-668cdbdefab3)
| Metal Gear Solid 3 |

## Credits
[@Lyall](https://codeberg.org/Lyall) for their amazing work making widescreen fix mods, and most importantly, the original creation of this mod!<br />
[@ShizCalev/Afevis](https://github.com/shizcalev) for long-term maintenance and contributing fixes.<br />
[@emoose](https://github.com/emoose), [@cipherxof](https://github.com/cipherxof), & [@Bud11](https://github.com/bud11) for contributing fixes. <br />
[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) for ASI loading. <br />
[inipp](https://github.com/mcmtroffaes/inipp) for ini reading. <br />
[spdlog](https://github.com/gabime/spdlog) for logging. <br />
[safetyhook](https://github.com/cursey/safetyhook) for hooking.
