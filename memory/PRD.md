# FOUNDATION — SCP Site Overseer (Terminal Edition)

## Problem statement
Originally an Unreal Engine 5.5 SCP game. **Pivoted** (user cannot import meshes / use the UE editor)
to a **standalone Python terminal game** that ports the "Site Overseer" management meta-game and can be
fully run and tested inside the container.

## Tech / constraints
- Pure Python 3.11 CLI using the `rich` library for the terminal UI.
- No engine, no 3D assets. Run via `python -m game.main` or `python play.py` from `/app`.
- JSON persistence in `/app/saves/` (`save_data.json`, `archives.json`).
- All lore/economy/progression data salvaged from the deprecated UE5 `FoundationDataLibrary.cpp`
  and `OverseerProgression.cpp` before those files were deleted.

## Implemented (2026-06)
- **Data layer** (`game/data/*.json` + `game/data.py`): 22 SCPs, 8 rogue MTF/GOI threats, 15 equipment
  items, 6 ranks, 3 personnel dossiers; research projects + 30 containment ops derived in code
  (mirrors the original C++ formulas exactly).
- **Progression** (`game/state.py`): two currencies (Credits / Research Credits), XP + 6-rank ladder
  with auto-promotion, owned equipment, completed research/ops, 4-slot loadout, breach record.
  Start: 500 Credits, 200 Research Credits, Junior Researcher (L0).
- **Site Overseer Hub** (`game/hub.py`): Research Division, Store, Containment Operations,
  Pre-Breach Loadout, Breach Response, Site-20 Personnel, Ranks & Rewards, Archives.
- **Text-based Breach** (`game/breach.py`): RNG survival event; survival chance =
  `60 − threat·8 + rank·5 + loadout_bonus` (clamped 10–95); flavour log + reward/penalty.
- **Archives** (`game/archives.py`): JSON-backed CRUD for player-authored custom SCPs; breachable
  customs appear in the Breach menu.
- **Cleanup**: deleted `/app/Source`, `/app/Config`, `/app/Content`, `Foundation.uproject` after porting.
- README rewritten for the terminal game.

## Verified
- Engine logic test (research gating, purchase gating, op RNG, rank-up, breach rewards, archive CRUD) — PASS.
- Scripted UI runs (main hub, ranks, personnel, breach deploy, quit/save) — PASS.

## Backlog / Next (P1/P2)
- P1: Per-SCP custom breach scenarios (branching choices, not just a single roll).
- P1: "New Game / reset save" option from the main hub.
- P2: ASCII art per anomaly; sound-free typewriter pacing for breach logs.
- P2: Achievements / end-game O5 victory screen.
- P2: Difficulty modes (Iron-man permadeath save wipe on breach failure).

## Files
```
game/{main,hub,breach,state,archives,data,ui}.py
game/data/{scps,equipment,ranks,rogue_mtf,procedures,personnel}.json
play.py ; saves/ (generated)
```

## License
CC BY-SA 3.0 (code + SCP content). See LICENSE / CREDITS.md.
