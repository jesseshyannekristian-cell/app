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

## Known limitations
- Not compiled (no UE editor in env). Must build in UE 5.5.
- AI movement is direct-steering (no NavMesh dependency) — fine for flat facility floors.
- NPC/SCP visuals are tinted cube placeholders; replace with real meshes/animations.

## Backlog / Next
- P1: Real meshes/animations + UMG HUD (sanity bar, keycard level, objective marker, prompts).
- P1: SCP-16829 "The TV in Time" (XI-87's mandate) with temporal-loop mechanic; SCP-049.
- P2: NavMesh + behavior trees for smarter faction AI and cover.
- P2: Save/seed sharing, difficulty scaling, audio (ambience, 173 sting, 096 scream).
- P2: Multi-floor facility + surface exterior for true open world.
