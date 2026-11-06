# GemTrio — Build Status & Roadmap

Last audited against commit `28ba5dc9` (plus the QoL update).

This file exists so nobody has to guess what is actually inside the ROM.
Everything in **Shipping now** can be reached by playing; everything in
**Not wired up yet** exists as source or documentation but is *not* compiled
into `pokeemerald.gba`.

---

## ✅ Shipping now

| Area | State | Where |
| :--- | :--- | :--- |
| Hoenn campaign | Complete, playable | `data/maps/*` (region `REGION_HOENN`) |
| 16-Badge level cap table (Hoenn rows) | Working, enforced | `src/battle_setup.c` |
| Badge → Level Cap announcement | Working | `data/scripts/gemtrio.inc`, 8 Hoenn Gym scripts |
| Non-destructive cap mode | Working | `include/config/gemtrio.h` |
| EV cap scaled over 16 Badges | Working | `src/caps.c` |
| HGSS-style Pokédex, following Pokémon, day/night, visible OW encounters | Working (expansion features, enabled) | `include/config/overworld.h`, `wild_encounter.h` |
| Reusable TMs, HM freedom, Repel menu | Working | configs + `pokeemerald` |
| Native launcher & Asset Extractor | Working (Ship of Harkinian model) | `GemTrio.exe`, `Play_GemTrio.bat` |
| Dedicated secure save manager | Working (`saves/` + auto backups) | `saves/gemtrio.sav`, `saves/backups/` |
| Native PC runner (SDL2) | Working | `bin/gemtrio-runner.exe` |

### Building

The build must run inside devkitPro's MSYS2 bash, because the Makefile calls
`/opt/devkitpro/...` paths:

```bash
bash -lc 'export DEVKITPRO=/opt/devkitpro
export DEVKITARM=/opt/devkitpro/devkitARM
export PATH=/opt/devkitpro/devkitARM/bin:/opt/devkitpro/tools/bin:$PATH
cd /d/Kaufee/projects/personal/pc-port/gemtrio
make -j8'
```

`python3` is required by the data generators (`tools/wild_encounters`). If
`python3: command not found` appears, put a `python3` shim on the MSYS2 `PATH`.
A 213 MB `EmeraldRecomp.exe` and a 32 MB ROM fit comfortably but note ROM usage
is already **~91 %** (`ld` prints the figure at link time) — any large addition
needs `SPECIES_ENABLED` pruning or asset compression.

---

## 🚧 Not wired up yet (the honest gaps)

### 1. Kanto is not in the Emerald ROM

`README.md` advertises **“16 Badges across Hoenn & Kanto”**. Today the ROM
contains **Hoenn only**. Verified by inspecting the linker map: no
`ViridianCity_Frlg` / `PewterCity_Gym_Frlg` symbols exist in `pokeemerald.map`.

Why: `tools/mapjson/mapjson.cpp` (`process_groups`, ~line 753) drops every map
whose `region` is not `REGION_HOENN` when building the Emerald version, and the
same filter applies to `layout_version != "emerald"` for layouts. The Kanto maps
and layouts are present in the tree but only compile into `make firered`.

To actually merge the two regions:

1. `tools/mapjson/mapjson.cpp`: accept `REGION_KANTO` maps and `frlg` layouts in
   the `emerald` version (or add a third `gemtrio` version that accepts both).
2. Include the FRLG tilesets referenced by the Kanto layouts
   (`data/tilesets/*`) in `graphics_file_rules.mk` / `data/tilesets/*.json`.
3. Remap the eight Kanto Gym scripts: they currently `setflag FLAG_BADGE01_GET`
   … `FLAG_BADGE08_GET` (`data/maps/PewterCity_Gym_Frlg/scripts.inc` etc.),
   which in the Emerald build are the **Hoenn** Badge flags. They must use
   `FLAG_KANTO_BADGE01_GET` … `FLAG_KANTO_BADGE08_GET`
   (`include/constants/flags.h`, currently `0x20`-`0x27`).
4. Add a travel link (ferry from Lilycove/Slateport to Vermilion) plus the
   `REGION_KANTO` map-section data so the Pokédex/region map and whiteout
   respawns switch correctly (`src/regions.c`, `src/region_map.c`).
5. Re-check `IS_FRLG` compile-time branches (`src/field_move.c`,
   `src/pokemon.c`) that assume one region per ROM.
6. Watch ROM budget: the 32 MB cart is 91 % full.

### 2. The C mod loader in `src/mod_loader/` is not linked into anything

`vfs.c`, `asset_override.c`, `json_data_loader.c`, `memory_map.c` are real,
well-structured host code, but:
- they are deliberately excluded from the GBA build (`Makefile`, commit
  `bb4c24bb`), and
- `EmeraldRecomp.exe` is a prebuilt binary with no hook for them — nothing ever
  calls `VFS_Init()` or `LoadAsset_Override()`.

So dropping files into the repository-root `mods/` folder currently has **no
effect**. The working mods are the recompiler's own packages under
`emerald_recomp_bin/mods/packages/**/manifest.toml` (currently the widescreen and
absolute-RTC packages), toggled from the launcher's **Mods** tab. Note those
manifests declare the *vanilla* Emerald SHA-1
(`f3ae0881…`), while this ROM hashes to a different value, so they are
presentation-level features of the runner, not ROM patches.

Next steps if true runtime mods are wanted:
- Ship a `gemtrio.enhancement.*` package that targets this ROM's SHA-1 and a
  plugin id the exe already knows, **or**
- Upstream a load hook into the recompiler so it calls `VFS_Init()` at boot and
  consults `LoadAsset_Override()` in the asset path, **or**
- Move mod loading to build time (JSON → generated C data) and stop advertising
  hot-swapping.

Until one of those lands, `mods/` and `src/mod_loader/` should be described as
**experimental / unlinked**, not as a shipping feature.

### 3. `rom.sha1` is stale

`rom.sha1` still contains the vanilla Emerald hash. It is only checked when
building with `COMPARE=1`, so it does not break `make`, but it misleads anyone
comparing hashes.

---

## 🔜 Nice next QoL candidates

- Level cap readout on the Trainer Card or in the Options menu.
- A `GemTrio` submenu on the Start menu gathering: current cap, EXP Share
  toggle, following-Pokémon toggle, text speed.
- Batch "use Max Repel" from the Bag.
- Type-chart viewer in the Pokédex.
- Rebind the recompiler's fast-forward/rewind keys to documented defaults.
