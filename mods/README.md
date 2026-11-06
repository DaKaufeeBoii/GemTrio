# GemTrio Modding Guide (Native PC Port)

> **Status: experimental / not yet linked.**
> The C mod loader in `src/mod_loader/` is complete and compiles, but it is
> excluded from the GBA build and `EmeraldRecomp.exe` (prebuilt) never calls
> `VFS_Init()` or `LoadAsset_Override()`. Files dropped into this folder do
> **not** affect the running game yet. The mods that *do* work today are the
> recompiler packages under `emerald_recomp_bin/mods/packages/**` toggled from
> the launcher's **Mods** tab. See `STATUS.md` for the two realistic paths to
> make this folder live.

The GemTrio native PC port allows loading user-created modifications dynamically at runtime from this `mods/` directory without requiring any recompilation of the executable.

---

## Directory Structure

```
mods/
├── sprites/       # Custom Pokémon, trainer, and overworld sprites (.png, .4bpp, .8bpp)
├── palettes/      # Custom color palettes (.pal, .gbapal)
├── data/          # JSON data patches (base_stats.json, encounter_tables.json, trainers.json)
└── sounds/        # Custom audio tracks and sound effects (.wav, .mid)
```

---

## 1. Asset Overrides (`/mods/sprites/`, `/mods/palettes/`, `/mods/sounds/`)

Any file placed inside these directories whose filename matches the in-game asset name (without file extension) will automatically override the game's original baked-in ROM asset.

### Example:
- **Bulbasaur front sprite:** Place custom image at `mods/sprites/gMonFrontPic_Bulbasaur.png`.
- **Custom Player palette:** Place custom palette at `mods/palettes/gTrainerPalette_Brendan.pal`.

The engine's `LoadAsset_Override` function computes the 32-bit FNV-1a hash of the requested asset and routes the read directly from this folder.

---

## 2. Runtime Data Patches (`/mods/data/`)

Modders can adjust game balance, encounter lists, and trainer rosters using simple `.json` files.

### `mods/data/base_stats.json`
```json
{
  "species": [
    {
      "id": 1,
      "hp": 50,
      "attack": 55,
      "defense": 55,
      "sp_attack": 70,
      "sp_defense": 70,
      "speed": 50
    }
  ]
}
```

---

## 3. GameShark / Action Replay Memory Cheats

To apply classic GameShark or Action Replay memory modifications, place a `cheats.txt` file in `mods/data/cheats.txt`:

```
# Format: <GBA_HEX_ADDRESS> <HEX_VALUE> [BYTE_WIDTH: 1, 2, or 4]
# Infinite Money (0x02000000 EWRAM base)
020244EC 000F423F 4
```

The PC port's address translation layer maps standard GBA 24-bit/32-bit addresses (`0x02XXXXXX` EWRAM, `0x03XXXXXX` IWRAM, `0x08XXXXXX` ROM) straight into native PC memory.
