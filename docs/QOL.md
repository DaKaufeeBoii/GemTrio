# GemTrio — Quality of Life Guide

This page lists every Quality-of-Life (QoL) feature currently compiled into
GemTrio and, for each one, **exactly how to see it in game**.

> Everything below is in the ROM today. Rebuild with `make -j8` after changing
> anything under `include/config/` or `data/`, then re-run `Play_GemTrio.bat`.

---

## 0. Launching

1. Double-click **`Play_GemTrio.bat`** in the repository root.
2. The launcher copies `pokeemerald.gba` (and the BIOS) next to
   `emerald_recomp_bin\EmeraldRecomp.exe`, writes `rom.cfg` / `bios.cfg` if they
   are missing, then starts the native recompiler.
3. If the ROM was rebuilt, the launcher picks up the new file automatically —
   no emulator or recompile step is involved.

---

## 1. 16-Badge Level Cap (the signature system)

Caps live in one place: `src/battle_setup.c` → `sGemTrioBadgeLevelCaps[]`, and
are mirrored in `README.md`. `include/config/gemtrio.h` controls the behaviour.

| Badges | Cap | Badges | Cap |
| :---: | :---: | :---: | :---: |
| 0 | 10 | 9 (Brock) | 53 |
| 1 (Roxanne) | 15 | 10 (Misty) | 56 |
| 2 (Brawly) | 19 | 11 (Lt. Surge) | 60 |
| 3 (Wattson) | 24 | 12 (Erika) | 64 |
| 4 (Flannery) | 29 | 13 (Koga) | 68 |
| 5 (Norman) | 31 | 14 (Sabrina) | 73 |
| 6 (Winona) | 33 | 15 (Blaine) | 78 |
| 7 (Tate & Liza) | 42 | 16 (Blue) | 85 |
| 8 (Juan) | 46 | Champion + 16 Badges | 100 |

**How to experience it**

1. Start a new game and check any party Pokémon: you cannot push past **Lv 10**
   before the first Badge — extra EXP is thrown away (`B_EXP_CAP_TYPE =
   EXP_CAP_HARD` in `include/config/caps.h`).
2. Beat **Roxanne** in Rustboro City.
3. Right after the Badge fanfare a new message appears:
   *“The party's LEVEL CAP is now 15!”* (`data/scripts/gemtrio.inc`).
4. Fight anything — EXP now flows again up to Lv 15, then stops.

### Non-destructive clamping (new)

`GEMTRIO_STRICT_LEVEL_CLAMP` in `include/config/gemtrio.h` is `FALSE` by
default. Previously the game silently **de-leveled** any party member above the
cap at the start of a battle; traded or gifted Pokémon above the cap lost levels
permanently. Now only EXP gain is capped. Set the option to `TRUE` to restore
the old strict behaviour.

### EV cap (new)

`GEMTRIO_EV_CAP_SCALES_WITH_BADGES` spreads the EV cap across all **16** Badges
instead of stopping at Hoenn's 8 (`src/caps.c`). Check a Pokémon's Summary →
EV/IV page after each Badge to watch the ceiling rise.

---

## 2. Battle QoL

| Feature | Config | How to see it |
| :--- | :--- | :--- |
| Target type indicators while choosing a move | `B_SHOW_TYPES = SHOW_TYPES_ALWAYS` | Start any battle, pick **Fight**, highlight a move — the enemy's types appear by its HP bar. |
| Move effectiveness indicator | `B_SHOW_EFFECTIVENESS` | Same screen: the PP readout is replaced by a super-effective / not-very-effective marker for seen species. |
| Fast HP drain and EXP bars | `B_FAST_HP_DRAIN`, `B_FAST_EXP_GROW` | Watch any damage or level-up animation. |
| Fast battle intro text | `B_FAST_INTRO_PKMN_TEXT` | Enter a wild battle; the “Wild X appeared!” text overlaps the send-out animation. |
| Move description in battle | `B_SHOW_MOVE_DESCRIPTION` | **Fight** → highlight a move; the description box updates. |
| Reusable TMs | `I_REUSABLE_TMS = TRUE` | Teach a TM, then open the Bag → TMs; the TM is still there. |
| Use evolution items from the Bag | `I_USE_EVO_HELD_ITEMS_FROM_BAG = TRUE` (new) | Get an Electirizer / Magmarizer class item, select it from the Bag, choose the Pokémon. No held-item trade dance. |
| Party-wide Exp. Share | `I_EXP_SHARE_ITEM = GEN_6` | Key item **Exp. Share** toggles party-wide EXP. |

---

## 3. Overworld QoL

| Feature | Config | How to see it |
| :--- | :--- | :--- |
| HM field moves without teaching them | `ScrCmd_checkfieldmove` (`src/scrcmd.c`) | Have any non-Egg Pokémon and walk up to a cuttable tree / water tile. A menu offers the field move even if no party member knows it. |
| Forget HM moves freely | `P_CAN_FORGET_HIDDEN_MOVE = TRUE` | Move Deleter, or any move-learning prompt, can overwrite an HM. |
| Reusable Repel with a menu | `I_REPEL_LURE_MENU = TRUE` | Use a Repel; when it wears off you are asked whether to use another, with a full list. |
| Run indoors | `OW_RUNNING_INDOORS = GEN_LATEST` | Hold **B** inside a house or Gym. |
| Day / night tint + dynamic palettes | `OW_ENABLE_DNS = TRUE`, `OW_TIMES_OF_DAY = GEN_LATEST` | Play at different real-world times; Route 101 looks different at night. |
| Following Pokémon | `OW_FOLLOWERS_ENABLED = TRUE` | Your lead party member walks behind you, bobs when idle, pops out of its Poké Ball. |
| Visible overworld encounters | `WE_OW_ENCOUNTERS = TRUE` | Walk in tall grass — wild Pokémon are rendered as sprites you can walk into. Shiny ones sparkle (`WE_OWE_SHINY_SPARKLE`). |
| No repeat map pop-up | `OW_HIDE_REPEAT_MAP_POPUP = TRUE` (new) | Walk out of and back into the same route: the location banner only shows the first time. |
| Defog / Rock Climb usable in the field | `OW_DEFOG_FIELD_MOVE`, `OW_ROCK_CLIMB_FIELD_MOVE = TRUE` | Castform-free Defog clearing and Rock Climb walls. |
| Choose from PC or party | `OW_CHOOSE_FROM_PC_AND_PARTY = TRUE` | Talk to a move tutor or an in-game trade NPC: the picker includes PC boxes. |
| Fast default text speed | `SetDefaultOptions` (`src/new_game.c`, new) | New game → **Options**: text speed already on Fast. |

---

## 4. Native PC features (from the recompiler, not the ROM)

These come from `emerald_recomp_bin\EmeraldRecomp.exe`, not from the GBA build:

- **Widescreen** — launcher → **Mods** → *Overworld Widescreen (Experimental)* →
  Fit / 16:9 / 21:9 / 32:9 → Apply.
- **Fast-forward / rewind** — configured in `config.ini`
  (`assist_fast_forward_multiplier`, `assist_rewind_key`).
- **Netplay** — launcher → **Netplay** tab (cable or wireless Union Room).
- **Controller support** — `keybinds.ini`; Xbox / DualShock / DualSense / Pro
  Controller are detected directly.

---

## 5. Tuning cheatsheet

| Want to change | Edit |
| :--- | :--- |
| Level caps, clamp mode, announcement | `include/config/gemtrio.h`, `src/battle_setup.c` |
| EXP / EV cap behaviour | `include/config/caps.h`, `src/caps.c` |
| Battle speed, type indicators | `include/config/battle.h` |
| Overworld toggles | `include/config/overworld.h` |
| Item toggles | `include/config/item.h` |

After editing, rebuild:

```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export PATH=/opt/devkitpro/devkitARM/bin:/opt/devkitpro/tools/bin:$PATH
make -j8
```
