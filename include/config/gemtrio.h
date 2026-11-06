#ifndef GUARD_CONFIG_GEMTRIO_H
#define GUARD_CONFIG_GEMTRIO_H

// ─────────────────────────────────────────────────────────────────────────────
// GemTrio tuning knobs
//
// Everything unique to GemTrio that a player or maintainer may want to tweak
// lives here instead of being hard-coded across several source files.
// ─────────────────────────────────────────────────────────────────────────────

// 16-Badge level cap progression (Hoenn 1-8, Kanto 9-16).
#define GEMTRIO_LEVEL_CAPS_ENABLED      TRUE

// How the cap is enforced:
//   FALSE - Non-destructive. The party-level cap variable is kept in sync so
//           EXP gain stops at the cap (B_EXP_CAP_TYPE), but no Pokémon is ever
//           de-leveled. Levels already earned (trades, gifts, events) are kept.
//   TRUE  - Strict. At the start of every battle, any party member above the
//           current badge cap is lowered to it. Vanilla GemTrio behaviour.
#define GEMTRIO_STRICT_LEVEL_CLAMP      FALSE

// Show "The party's LEVEL CAP rose to N!" right after a Gym Badge is earned.
#define GEMTRIO_LEVEL_CAP_NOTIFY        TRUE

// Level cap once the player becomes Champion (all 16 Badges, Hall of Fame).
#define GEMTRIO_POSTGAME_LEVEL_CAP      100

// Scale the EV cap across all 16 badges instead of stopping at Hoenn's 8.
// With this FALSE the old 8-badge curve (15/17 of MAX_TOTAL_EVS at 8 Badges)
// is used unchanged.
#define GEMTRIO_EV_CAP_SCALES_WITH_BADGES TRUE

#endif // GUARD_CONFIG_GEMTRIO_H
