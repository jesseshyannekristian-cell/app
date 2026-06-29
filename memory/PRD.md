# FOUNDATION — SCP Game (Unreal Engine 5.5, C++)

## Problem statement
"Making an SCP game in Unreal Engine 5 — need a world generated with underground containment facilities."
Follow-up: add MTF XI-87 (from uploaded dossier) as a faction, make it open-world, add more factions
(Chaos Insurgency, UIU, Wondertainment), add more SCPs from the wiki with ways to contain them, and random encounters.

## Tech / constraints
- UE 5.5, C++ source project. Editor cannot run in this environment — delivered as ready-to-open/compile source.
- Zero-art-asset playable: all geometry procedurally built from engine cube mesh.
- Legacy input mappings (Config/DefaultInput.ini); GameMode wired via Config/DefaultEngine.ini.

## Implemented (as of 2026-06)
- Procedural modular facility generator (connector stitching, shapes, overlap rejection, sealing).
- First-person character: flashlight, sprint, interaction, keycards, health, sanity/blink component.
- Factions: Foundation, MTF (XI-87 "Binge Watchers" named roster from dossier), Chaos Insurgency, UIU,
  Wondertainment, Civilian, Anomaly — with relation matrix + autonomous combat NPCs (AFactionCharacter).
- SCPs: 173 (observe-freeze), 096 (face-view enrage), 106 (phase/teleport), 682 (tanky Keter), generic base.
  Containment metadata (class/procedure/clearance) + AContainmentTerminal re-containment loop.
- Random encounters / open-world spawner (AEncounterManager): patrols, breaches, civilians; seeds MTF squad.
- Objective loop: keycards → exit zone (clearance-gated) → escape; death/escape regenerates facility.

## Implemented (Site Overseer meta-game — 2026-06, NEW)
- **Title screen** ("SCP SITE OVERSEER") in C++/Slate: provided art (`Content/UI/TitleScreen.png`)
  loaded at runtime as letterboxed backdrop, real buttons over painted START/OPTIONS/CREDITS.
  Options + Credits panels (Credits shows CC BY-SA 3.0 attribution). `MainMenuGameMode` is now the
  default GameMode; `MainMenuPlayerController` drives title → hub → breach flow.
- **Site Overseer Hub** (`SSiteOverseerHub`) with 6 tabs: Research Division, Store,
  Containment Operations, Breach, Site-20 Personnel, Ranks & Rewards.
- **Site-20 Personnel dossiers** (lore/UI only): GUNZ (O5 Council), CHEMOSIC (Site Supervisor /
  ex-MTF XI-87 Commander), Head Researcher — `FPersonnelData` in `UFoundationDataLibrary`.
- **Repo cleaned to game-only**: removed unused React `frontend/` + FastAPI `backend/` template folders.
- **Two-currency economy**: Credits (gear) + Research Credits (research only). Persisted via
  `UOverseerProgression` (GameInstanceSubsystem) → `UOverseerSaveGame` ("OverseerSave").
- **Research → Store gating**: every level-locked item has an auto-generated research project; item
  stays LOCKED in store until researched (+ rank). Containment-procedure research projects added.
- **Containment Operations** (menu-driven, separate from Breach): ops auto-generated for ALL SCPs and
  ALL rogue MTF/GOI threats; gated by rank/equipment/procedure; reward Credits/RC/XP.
- **Ranks & Rewards**: 6-rank ladder (Junior Researcher → O5 Overseer OMEGA BLACK) with promotion
  rewards + auto rank-up; includes a Breach record section. Breach survival/failure feeds rewards.
- **Data layer** `UFoundationDataLibrary`: ~21 SCPs, 8 rogue MTF/GOI, field+research equipment, ranks.

## Files (meta-game)
- Public/Private: `FoundationTypes.h`, `FoundationDataLibrary.*`, `OverseerSaveGame.h`,
  `OverseerProgression.*`, `SMainMenuWidget.*`, `SSiteOverseerHub.*`,
  `MainMenuPlayerController.*`, `MainMenuGameMode.*`. Build.cs adds Slate/SlateCore/UMG.

## Known limitations
- Not compiled (no UE editor in env). Must build in UE 5.5.
- Title PNG loaded from disk at runtime (editor/PIE OK); for packaged builds import as Texture2D
  or add `Content/UI` to packaging non-asset dirs.
- AI movement is direct-steering (no NavMesh dependency) — fine for flat facility floors.
- NPC/SCP visuals are tinted cube placeholders; replace with real meshes/animations.

## Backlog / Next
- P1: Real meshes/animations + in-breach UMG HUD (sanity bar, keycard, objective marker, prompts).
- P1: SCP-049 entity; make Containment Operations interactive (success chance) instead of instant.
- P2: Spend owned equipment in live Breach (loadout select before deploy).
- P2: NavMesh + behavior trees; audio (ambience, 173 sting, 096 scream); multi-floor + surface.

## Done recently (2026-06)
- SCP-16829 "The TV in Time" (`ASCP16829`): dimensional-travel interact, temporal-loop rewind,
  carcinogenic exposure (ties GUNZ/CHEMOSIC dossiers). Added to Breach roster, Operations (gated by
  `proc_16829` research) and a containment-procedure research project. Spawned once per facility.
