# FOUNDATION — SCP Site Overseer (Terminal Edition)

> ## ⚠️ CURRENT STACK = REACT NATIVE (EXPO) MOBILE APP + FASTAPI (NOT the terminal game)
> The project pivoted from UE5 → Python CLI → **React Native (Expo Web) mobile app**. The CLI text
> below is legacy/historical. Active code: `/app/frontend` (Expo) + `/app/backend` (FastAPI). Legacy
> `/app/game/` CLI can be deleted (P2).
>
> ### Mobile app — implemented (2026-06-29)
> - **Graphic UI overhaul** (replaced the CRT/ASCII terminal skin): dark sci-fi "control-room"
>   dashboard per user's reference template — card layout, red accent rules, bordered classification
>   badges, solid-yellow primary CTAs, monospace data + bold sans titles. Design system in
>   `src/theme.js` + `src/ui.js` (Screen, Card, Btn, Badge, SectionTabs, Meter, Toast).
> - **5-icon bottom tab bar**: TERMINAL(Home) / ARCHIVE(Archives) / ENCOUNTERS(Operations+Breach) /
>   ARMORY(Store+Research+Loadout) / DOSSIER(Personnel+Awards+System). Section pills switch siblings.
>   Web URL deep-links added via React Navigation `linking` (/, /store, /archives, /operations, etc.).
> - **Resilient loading**: app boots instantly from bundled `src/bootstrapData.json` + `DEFAULT_STATE`;
>   `src/store.js` background-syncs live config/state with retry + abort-timeout (`src/api.js`); autosave
>   only after first sync settles (no clobber). Fixes the prior "bootstrap failed / stuck booting"
>   cold-start issue.
> - **CC BY-SA 3.0**: `/app/LICENSE` + `/app/CREDITS.md` present; in-app attribution on Home footer.
> - Verified via testing_agent iteration_6: 30/30 frontend checks passed, no bugs.
> - **Note**: headless browsers in this env can stall on reading API response BODIES on cold start
>   (backend returns 200, body buffers) — hence the bundled-config render model. Real browsers load fine.


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
- Feature batch (branching scenarios, art, achievements, Iron-man, save-on-interrupt, rng bounds) — PASS.
- Content+difficulty batch: all 22 Site-20 SCPs have branching scenarios; Easy/Normal/Hard tiers
  (+15/0/-15% breach survival) shown in header + assessment, persisted, preserved on Iron-man wipe;
  `_branching` future-proofed for >2 options; New Game has a single summary confirm before wipe.
- Test suites: `backend/tests/test_foundation_scp.py` + `test_new_features.py` = **68 passing**
  (run with `SCP_NO_DELAY=1 python -m pytest backend/tests/ -q`).

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
