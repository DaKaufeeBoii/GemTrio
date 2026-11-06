# GemTrio — Native PC Port

<p align="center">
  <img src="graphics/pokemon/rayquaza/anim_front.png" alt="GemTrio Logo" width="128" />
  <br>
  <b>An interconnected Pokémon journey spanning 16 Badges across Hoenn & Kanto, recompiled natively for PC.</b>
  <br>
  <sub>High-performance static recompilation • 16-Badge Level Caps • Gen 9 Battles • Modern QoL • Hot-Swappable Mod Engine</sub>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows%20x64%20|%20Native%20PC-0078D6?style=flat-square&logo=windows" alt="Platform" />
  <img src="https://img.shields.io/badge/Engine-EmeraldRecomp%20+%20pokeemerald--expansion-306998?style=flat-square" alt="Engine" />
  <img src="https://img.shields.io/badge/Badges-16%20(Hoenn%20+%20Kanto)-FFD700?style=flat-square" alt="Badges" />
  <img src="https://img.shields.io/badge/Battle%20Engine-Gen%209%20Mechanics-E3350D?style=flat-square" alt="Battle Engine" />
  <img src="https://img.shields.io/badge/Modding-Hot--Swappable%20VFS-4CAF50?style=flat-square" alt="Modding" />
</p>

---

## 📖 Overview

**GemTrio** is a custom Pokémon experience that bridges **Hoenn and Kanto** into a single cohesive world with 16 gym badges, built upon the high-performance `pokeemerald-expansion` codebase and recompiled into a standalone 64-bit native PC executable via **Emerald Recomp**.

By breaking free from GBA hardware constraints while preserving authentic mechanics, GemTrio delivers native 60 FPS performance, arbitrary resolutions including widescreen and ultrawide, zero emulation latency, and a built-in C-level dynamic modding system that allows hot-swapping assets, stats, and scripts without touching the game compiler.

---

## ✨ Key Features

### ⚔️ Modern Battle Engine & Mechanics
* **Physical / Special Split**: Every move is dynamically split based on modern core-series classifications.
* **Fairy Typing & Gen 9 Type Chart**: Fully updated type matchups and resistances.
* **Modern Movepools & Abilities**: Integrated modern expansion abilities, moves, and battle effects.

### 🏆 16-Badge Strict Level Cap Progression
To preserve challenge and prevent overleveling across both regions, the battle engine strictly enforces progression caps based on the player's total badge count (`src/battle_setup.c`):

| Region | Badge # | Representative Gym | Level Cap |
| :--- | :---: | :--- | :---: |
| **Hoenn** | 1 | Stone Badge (Roxanne) | **Lv 15** |
| | 2 | Knuckle Badge (Brawly) | **Lv 19** |
| | 3 | Dynamo Badge (Wattson) | **Lv 24** |
| | 4 | Heat Badge (Flannery) | **Lv 29** |
| | 5 | Balance Badge (Norman) | **Lv 31** |
| | 6 | Feather Badge (Winona) | **Lv 33** |
| | 7 | Mind Badge (Tate & Liza) | **Lv 42** |
| | 8 | Rain Badge (Wallace/Juan) | **Lv 46** |
| **Kanto** | 9 | Boulder Badge (Brock) | **Lv 53** |
| | 10 | Cascade Badge (Misty) | **Lv 56** |
| | 11 | Thunder Badge (Lt. Surge) | **Lv 60** |
| | 12 | Rainbow Badge (Erika) | **Lv 64** |
| | 13 | Soul Badge (Koga/Janine) | **Lv 68** |
| | 14 | Marsh Badge (Sabrina) | **Lv 73** |
| | 15 | Volcano Badge (Blaine) | **Lv 78** |
| | 16 | Earth Badge (Blue) | **Lv 85** |
| **Postgame** | — | Champion & Battle Frontier | **Lv 100** |

*EXP gain stops at the cap until the next Badge is earned; the numbers live in `src/battle_setup.c` and are announced in-game after every Badge.*

> ⚠️ **Region status:** the current ROM contains the **Hoenn** campaign only.
> The eight Kanto Gyms, their flags (`FLAG_KANTO_BADGE01_GET`…) and the cap
> values for Badges 9-16 are all implemented, but the Kanto maps are filtered
> out of the Emerald build by `tools/mapjson`. See [`STATUS.md`](STATUS.md) for
> the exact steps to merge the regions.

### 🛠️ Modern Quality-of-Life (QoL)
* **Reusable TMs**: Technical Machines are never consumed upon use.
* **Gen 6+ Team Exp. Share**: Toggleable party-wide experience distribution.
* **HM Field Move Freedom**: Pokémon can use Cut, Surf, Fly, Strength, Flash, Rock Smash, Waterfall, and Defog in the field as long as an eligible Pokémon is in your party—**without permanently occupying a moveset slot**.
* **Level Cap feedback**: Earning a Badge announces the new party Level Cap in-game (`data/scripts/gemtrio.inc`) and applies it immediately instead of at the next battle.
* **No silent de-leveling**: the cap is enforced through EXP only by default—traded or gifted Pokémon above the cap keep their levels (`GEMTRIO_STRICT_LEVEL_CLAMP` in `include/config/gemtrio.h`).
* **Battle info at a glance**: target type indicators and move effectiveness are always shown while choosing a move.
* **PC-friendly defaults**: new saves start on Fast text speed.
* **Evolution items from the Bag**: Electirizer-class items are usable without a held-item trade.
* **Fewer interruptions**: no location banner when re-entering the same map section, plus the reusable-Repel menu.

➡️ **Every QoL feature, with how to see it in game, is documented in [`docs/QOL.md`](docs/QOL.md).**

### 🌿 Living World & Overworld Immersion
* **Day / Night System**: Dynamic lighting based on the real-time clock (RTC).
* **Dynamic Overworld Palettes (DOWP)**: World tiles and entities naturally reflect current time and weather.
* **Following Pokémon**: Your lead party member follows behind you in the overworld.
* **Visible Overworld Encounters**: Wild Pokémon visibly spawn in grassy routes and caves.

### 🖥️ Native PC Port Architecture & Features
* **Ship of Harkinian-Style Launcher (`GemTrio.exe`)**:
  * Clean legal asset extraction model: requires the player to provide an official *Pokemon - Emerald Version (USA)* ROM (`f3ae0881...`) once for authentic asset extraction.
  * Builds the enhanced GemTrio build **once** into `game/gemtrio.gba`, generating a verified build manifest (`game/build_info.json`).
* **Dedicated Save File Management (`saves/`)**:
  * Saves are stored strictly in `saves/gemtrio.sav` (isolated from binaries and working trees).
  * Save states live in `saves/states/`.
  * **Automatic Session Backups**: Automatically takes timestamped snapshots in `saves/backups/` before and after every play session. You never lose a save to crashes or corruption.
  * Built-in save manager buttons in the launcher to open the folder, create instant manual backups, or restore past snapshots.
* **Native SDL2 PC Runner**:
  * Hardware-accelerated 60 FPS gameplay with zero emulation buffer lag.
  * Configurable window scaling (2x, 3x, 4x, 5x) or borderless Fullscreen (`F11`).
  * Direct plug-and-play for Xbox, PlayStation (DualShock / DualSense), Nintendo Switch Pro, and standard controllers.
  * Fast-forward (hold `Left Shift` or controller RT).
  * Quick save state (`F5`) and quick load state (`F8`).

---

## 🧩 Dynamic Mod Support Architecture

GemTrio features an external mod loader built into the C/C++ runtime layer (`src/mod_loader/`):

```
gemtrio/
├── mods/
│   ├── sprites/         <-- Drop replacement .png files here
│   ├── palettes/        <-- Drop replacement palette files here
│   ├── data/            <-- Drop JSON data patches (base_stats.json, trainers, encounters)
│   ├── sounds/          <-- Drop replacement audio and cry files
│   └── README.md
```

### Mod Capabilities:
1. **Asset Interception Hook (`LoadAsset_Override`)**:
   Intercepts game asset requests at runtime. If an asset matching the path or hash exists inside `/mods/`, the game loads the external file dynamically without modifying or recompiling the game executable.
2. **Virtual File System (VFS)**:
   Scans the local `/mods/` directory on startup and constructs a fast FNV-1a hash table for near-instant path resolution.
3. **JSON Runtime Data Patching (`cJSON`)**:
   Modders can drop JSON files into `/mods/data/` (such as `base_stats.json`) to alter Pokémon base stats, typings, catch rates, abilities, and level-up moves on the fly.
4. **GameShark & Static Memory Translation**:
   Built-in translation layer mapping original GBA addresses (EWRAM `0x02000000`, IWRAM `0x03000000`, etc.) to the native host memory space, enabling external cheat files and runtime memory modification.

---

## 🚀 Getting Started

### Prerequisites (For Running)
* Windows 10 / 11 (64-bit)
* Official Game Boy Advance BIOS file (`gba_bios.bin`)

### Quick Play
1. Double-click **`Play_GemTrio.bat`** (or run **`GemTrio.exe`**).
2. On first launch, the wizard will ask you to select your official *Pokemon - Emerald Version (USA)* ROM for authentic asset extraction (just like *Ship of Harkinian* / *SM64-PC*).
   *(It automatically detects `..\roms\pokemon-emerald.gba` if present!)*
3. Click **EXTRACT ASSETS & BUILD GEMTRIO**. This builds the enhanced game once into `game/gemtrio.gba`.
4. Click **PLAY GEMTRIO** to start! Your progress is continuously and safely backed up to `saves/`.

---

## 🔨 Building From Source

To compile the ROM and host tools from source code:

### Requirements
* [devkitPro](https://devkitpro.org/) with `devkitARM` (`arm-none-eabi-gcc`)
* Python 3.10+ (with UTF-8 enabled)
* GNU Make

### Build Steps
```bash
# Set environment variables for devkitARM
$env:DEVKITPRO = "C:\devkitPro"
$env:DEVKITARM = "C:\devkitPro\devkitARM"
$env:PATH = "C:\devkitPro\devkitARM\bin;" + $env:PATH
$env:PYTHONUTF8 = "1"

# Compile host tools and the 32MB ROM in parallel
make -j8
```

The build outputs:
* `pokeemerald.gba` — Complete 32 MB GBA ROM cart image
* `pokeemerald.elf` — Full ELF executable with symbol maps
* `pokeemerald.map` — Linker symbol and address map

Running `Play_GemTrio.bat` automatically syncs the newly compiled ROM to the PC recompiler runner.

---

## 📁 Repository Structure

```
GemTrio/
├── GemTrio.exe                 # Native Windows GUI launcher (SoH-style asset extractor & hub)
├── Play_GemTrio.bat            # One-click launcher script
├── Makefile                    # Multi-core build pipeline
├── game/                       # Extracted & built game output
│   ├── gemtrio.gba             # Enhanced GemTrio build with all custom features
│   └── build_info.json         # Asset extraction manifest & SHA-1 records
├── saves/                      # Dedicated save storage
│   ├── gemtrio.sav             # Active battery save
│   ├── backups/                # Automated historical timestamped backups
│   └── states/                 # Quick save states (F5/F8)
├── bin/                        # Portable native SDL2 runner & BIOS
│   ├── gemtrio-runner.exe      # Native PC runner
│   └── gba_bios.bin            # GBA BIOS
├── include/                    # Engine headers and config toggles
│   ├── config/                 # Feature configuration (battle, caps, item, overworld)
│   └── constants/              # Game constants and badge definitions
├── src/                        # Game source code
│   └── battle_setup.c          # 16-badge level cap enforcement hooks
└── tools/                      # Native asset compilers (gbagfx, compresSmol, scaninc, etc.)
```

---

## 📜 Credits & Acknowledgments

* **Rom Hacking Hideout (RHH)** — For the outstanding [`pokeemerald-expansion`](https://github.com/rh-hideout/pokeemerald-expansion) base.
* **pret** — For the foundational [`pokeemerald`](https://github.com/pret/pokeemerald) disassembly project.
* **mstan & R.A.I.D. Community** — For [`EmeraldRecomp`](https://github.com/mstan/EmeraldRecomp) and the `gbarecomp` static recompilation framework.
* **Dave Gamble & Contributors** — For the lightweight [`cJSON`](https://github.com/DaveGamble/cJSON) parser.

---

## ⚖️ Disclaimer

*GemTrio is an educational, non-commercial fan-made project developed under fair use. Pokémon and Pokémon character names are trademarks of Nintendo, Creatures Inc., and Game Freak. This repository does not distribute copyrighted ROMs or proprietary Nintendo BIOS files.*
