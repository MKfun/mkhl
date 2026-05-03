MK: Half-life
===============================
<img src='logo.png'></img>


Based on [BugfixedHL-Rebased](https://github.com/tmp64/BugfixedHL-Rebased)


Features
--------
 ### Gameplay
  - Event on death: ```mp_eventondeath```
    - Grenades: when victim was killed while holding grenade, it will drop and explode\
    - Gauss: when victim was killed while holding SecondaryAttack, there will be final shot like victim shoot with PrimaryAttack
    - Satchel: explode victim's currently dropped satchels
  - New weapons:
    - Bumpmine: like in csgo DZ gamemode, but wont damage you on launch ```weapon_bumpmine```
  - New gamemodes:
    - Flying shotguns: only shotguns and bumpmines. Nothing else. Every 20 seconds players earn new bumpmine ```sv_flyingshots 1```
    - Hazardous zone™: Danger zone, but if it was in Half-Life. ```sv_hazardz 1```
  - Powerups: classic Quake (DMC) powerups.
  - Movement:
    - ```mp_ultrakill_additions``` enables some ultrakill additions (wow) like fast-fall (duck+use) and damaging with fast-fall (just hold duck and fly ont top of enemy)
 ### ClientSide
 - Same as BHL-Rebased but with some spicy extras
 - ~~Custom vgui2 csgo-like hud from [this pr](https://github.com/tmp64/BugfixedHL-Rebased/pull/280) ```hud_custom 1```~~ DEPRECATED
 - Custom web-like RocketHUD that anyone can customize! Thats pretty easy, just try it! ```rocket_enable 1```
 - With above: There are headshots, and killstreaks in hud like in csgo.
 - Viewmodel extras: QuakeGun™ and sway (weaponlag, like source games vm) ```cl_quakeguns 1``` and ```cl_weaponlag_enable 1```
 - freeze panel (only servers with mkhl supported, or there will be soon metamod plugin for vanilla servers)
 - SOON - custom gameui (big thanks to ScriptedSnark)
 ### SDK Features
 - Crash handler with libbacktrace (libbacktrace-dev package needed) ```-DWITH_LIBBT_CLIENT=1``` or ```-DWITH_LIBBT_SERVER=1```


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

###### for MK:HL
- tmp64 and all contributors for this nice HLSDK base.
- ReHLDS for [ReGameDLL_CS](https://github.com/rehlds/ReGameDLL_CS), i really inspired by their code.
- As well as CS server code, thanks to [Velaron](https://github.com/velaron) for his [CS16Client](https://github.com/Velaron/cs16-client) where i got the client code for headshots.
- ScriptedSnark for help in [ReGameUI project](https://github.com/MKfun/mkhl/tree/regameui).
- [Nillerusr](https://github.com/nillerusr) for the [source-engine](https://github.com/nillerusr/source-engine) repo, where i got some code from sdk part (killstreak).
- [FWGS community](https://github.com/FWGS), for help in some moments.
- [LWSS for kisak-strike](https://github.com/SwagSoftware/Kisak-Strike/) for RmlUI base integration into Source Engine, got some code from it, and partially rewrote it and updated.
- MK Community, that im working for. They're just my friends.
