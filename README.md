# ACPR Input History

Input history mod for Guilty Gear XX Accent Core Plus R (Steam) via [GearLoader](https://github.com/YouKnow232/GearLoader) by [YouKnow232](https://github.com/YouKnow232/).

It uses the Replay Mode input history to render the inputs on Training Mode. It was designed to be used in training mode - specifically in both Training and Online Waiting Modes - so it doesn't work in other modes.

<div align="center">
  <img src="./docs/img/Preview1.jpg" width="480" align="center" alt="Input History displayed along side the normal ggxxacpr HUD"/>
  <br/>
</div>


## Installation

* Download and install [GearLoader](https://github.com/YouKnow232/GearLoader) v1.1.1 or newer.
* Download the archive from [releases](https://github.com/Alquimas/ACPR_InputHistory/releases)
* Unzip it and place the mod folder into the "mods" folder

```text
Guilty Gear XX Accent Core Plus R/
└── mods/
    └── ACPR_InputHistory/
```

## Settings
<div align="center">
  <img src="./docs/img/Preview2.jpg" width="480" align="center" alt="Settings Menu"/>
  <br/>
</div>

This mod features a settings menu that can be found at "Pause Menu -> Help & Options -> Mod Settings".

## Training frame stepping
<div align="center">
  <img src="./docs/img/Preview3.jpg" width="480" align="center" alt="Settings Menu"/>
  <br/>
</div>

While the mod is active, press the configured `EnemyWalk` binding with
the game paused and the pause menu hidden to advance one update. This is
available only in the Training and Online Waiting modes supported by this mod.

Some examples of usage are:

- Pressing `EnemyWalk` alone so the next input is neutral;
- Holding `P` and then pressing `EnemyWalk` so the next input is `P`;
- And so on.

This feature was not tested with P2 inputs, and feedback for it is welcome.

Backwards stepping is currently not supported.

## Build Instructions
Build instructions are similar to [GearLoader's](https://github.com/YouKnow232/GearLoader).

### Prerequisites
* CMake v3.21 or later
* gcc v15.2.0 or later
    * i686-w64-mingw32-g++

For Windows users:
* MSYS2 environment

### Instructions
Run `Compile.sh` from the project root directory (in a MINGW32 shell for Windows users). You can set the environment variable `GGXXACPR_MOD_DIR` and the `Compile.sh` script will copy the build output to that directory.
