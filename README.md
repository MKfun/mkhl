ReGameUI (bhl-rb based)
===============================

### THIS BRANCH SHOULD BE USED ONLY FOR BUILDING GAMEUI, FOR SDK PLS USE MASTER BRANCH

ReGameUI is my project, that implementing gameui.so (like base panel, options, etc), but open sourced unlike valve's


Roadmap
--------
- [x] Engine starts (no crashes)
- [x] Console
- [x] BasePanel shows
- [x] background image
- [x] Multiplayer dialog
- [x] Singleplayer dialog
- [x] Nice menu (not vanilla)
- [x] FIXUP ServerBrowser scheme
- [ ] Builds on windows
- [x] ifdefs for MKHL

Bonus roadmap
--------------
- [ ] Animated bgs
- [ ] RocketUI
Technical moments
------------------
 - The code may be ugly, but thats a source beta code, srry
 - gameui lib will NOT load from valve_addon. Replace original library (backup first!)

SDK changes
-----------

BugfixedHL has been integrated into Valve's latest HLSDK and has been refactored:

- CMake as build system (instead of Makefiles and VS projects).
- Source code formatted to one style with clang-format.
- Organized source code files:
  - HLSDK sources moved to */src/*.
  - */cl_dll*, */dlls* and */game_shared* moved to */src/game/*.
  - */game_shared* cleaned up from unused code.
  - Removed */utils* completely (used to contain utilities like map and sprite compilers).
- Client sources refactoring:
  - VGUI1 replaced by VGUI2 (like in CS1.6).
  - Moved all VGUI code to *client/vgui*.
  - Moved all HUD elements to *client/hud*, each of them now has its own *.h* file.
  - Replaced HUD messages and commands macros with templates.
  - HUD elements are no longer referenced in *hud.h* (improves compilation times when changing *hud/\*.h*.
  - Added `ConVar` class for easier convar creating with description support (like in Source SDK).
  - Removed unused code.
- Fixed include guards in common header files.
- Documented engine APIs (thanks to [Solokiller](https://github.com/Solokiller)).


Supported game versions
-----------------------

Type `version` in the console. You will see something like this.

```
] version 
Protocol version 48
Exe version 1.1.2.2/Stdio (valve)
Exe build: 15:17:55 Jul 24 2019 (8308)
```

*8308* in the last line is your engine version.

| Engine version | Status          |
| :------------: | --------------  |
| 3xxx           | Not supported   |
| 4554           | Supported       |
| 8xxx+          | Supported       |


Reporting Issues
----------------

If you encounter an issue while using BugfixedHL, first search the [issue list](https://github.com/tmp64/BugfixedHL-Rebased/issues)
to see if it has already been reported. Include closed issues in your search.

If it has not been reported, create a new issue with at least the following information:

- a short, descriptive title;
- a detailed description of the issue, including any output from the command line or game console;
- steps for reproducing the issue;
- your system information\*;
- the `version` output from the in-game console;
- the `about` output from the in-game console.

Please place logs either in a code block (press `M` in your browser for a GFM cheat sheet) or a [gist](https://gist.github.com).

\* The preferred and easiest way to get this information is from Steam's Hardware Information viewer from the
menu (`Help -> System Information`). Once your information appears: right-click within the dialog, choose `Select All`,
right-click again, and then choose `Copy`. Paste this information into your report, preferably in a code block.


Conduct
-------

There are basic rules of conduct that should be followed at all times by everyone participating in the
discussions.  While this is generally a relaxed environment, please remember the following:

- Do not insult, harass, or demean anyone.
- Do not intentionally multi-post an issue.
- Do not use ALL CAPS when creating an issue report.
- Do not repeatedly update an open issue remarking that the issue persists.

Thanks
------

- Lev for creating [the original BugfixedHL](https://github.com/LevShisterov/BugfixedHL) that this project is based on.
  - Valve for HLSDK release.
  - Willday for his HLSDK patch.
  - BubbleMod and Bigguy from hlpp.thewavelength.net for parts of spectator code.
  - Uncle Mike from hlfx.ru for his Xash3D engine which was very helpful in hard moments.
  - KORD_12.7 for constant helping and nice suggestions.
  - Martin Webrant (BulliT) for releasing AG source code.
  - JetBrains company for free access to great developer tools.
- Solokiller for [Half-Life Enhanced](https://github.com/Solokiller/HLEnhanced) and GoldSrc engine research.
- AGHL.RU community for bug reporting and suggestions.
- YaLTeR and contributors for [OpenAG](https://github.com/YaLTeR/OpenAG) features
