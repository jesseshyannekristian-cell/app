# FOUNDATION — SCP Containment Breach (Unreal Engine 5.5)

A first-person SCP horror game with a **procedurally generated underground containment facility**, multiple **factions**, a roster of **anomalies (SCPs) with containment mechanics**, and **random open-world encounters**.

> ⚠️ This repository is a **C++ source project**. It is generated as ready-to-open Unreal source — it must be opened and compiled inside **Unreal Engine 5.5** (this environment cannot run the UE editor). Everything is written so the game is **playable the moment you press Play**, with **zero art assets required** (all geometry is built procedurally from the engine cube mesh).

---

## 1. Requirements
- **Unreal Engine 5.5** (install via the Epic Games Launcher).
- **Visual Studio 2022** (Windows) or **Xcode** (macOS) / **Rider** with C++ game dev workload.

## 2. First-time setup
1. Right-click `Foundation.uproject` → **Generate Visual Studio project files**.
2. Open the generated `Foundation.sln` (or open `Foundation.uproject` directly and let it compile).
3. Build the **Development Editor** target.
4. The editor opens. Create a level (or use any empty level):
   - **File → New Level → Empty Level** (or Basic).
   - Add a **Directional Light** + **Sky Light** if you want some ambient light. The facility is intentionally dark — the player has a flashlight (press **F**).
   - (Optional) Drop a **PlayerStart** anywhere; the GameMode repositions the player into the generated start room automatically.
   - **Save** the level and set it as the editor/default map (**Project Settings → Maps & Modes** if desired).
5. Press **Play**. The facility generates, the MTF squad deploys, and encounters begin.

The default GameMode is already wired in `Config/DefaultEngine.ini`
(`GlobalDefaultGameMode = /Script/Foundation.FoundationGameMode`), so **any level** runs the full game loop.

## 3. Controls
| Action | Key |
|---|---|
| Move | `W A S D` |
| Look | Mouse |
| Jump | `Space` |
| Sprint | `Left Shift` |
| Interact (pickups, doors, terminals) | `E` |
| Flashlight | `F` |

---

## 4. What's implemented

### Procedural facility generator (`AFacilityGenerator`)
- Stitches **modular rooms** connector-to-connector (`URoomConnector`, forward axis = doorway).
- Room shapes: **Straight, Corner, T-Junction, Cross, Dead-End, Spawn** (`ERoomShape`).
- Each room builds its own floor/ceiling/walls (with door gaps) procedurally — swap the cube meshes for your own modular kit later.
- AABB overlap rejection prevents rooms intersecting; dangling openings are sealed so the facility is enclosed.
- Populates the facility with keycards, **containment terminals**, an **exit zone**, and SCPs.

### First-person player (`AFoundationCharacter`)
- Camera + toggleable flashlight, sprint, interaction trace.
- **Sanity / blink system** (`USanityComponent`): involuntary blinks (eyes close) happen more often as sanity drops — critical against SCP-173.
- Health + damage (faction gunfire can hurt you).
- Keycard inventory (`KeycardLevel`).

### Factions (`EFaction`, `UFactionLibrary`, `AFactionCharacter`)
- **SCP Foundation** (the player) & **MTF** — allied.
- **MTF XI-87 "Binge Watchers"** — your allied squad, seeded from the uploaded dossier with named operatives: *Lead Agent Marcus "Loopbreaker" Kane, Deputy Elena Voss, Agent Riley "Static" Torres, Agent Jamal Reyes, Agent Sophia Laurent,* and *Kris "Gunz" (O5-00 Attached)* (tougher, higher DPS).
- **Chaos Insurgency** — hostile to Foundation/MTF/UIU.
- **UIU (Unusual Incidents Unit)** — jurisdictional rival, hostile to Chaos & anomalies.
- **Dr. Wondertainment** — neutral/eccentric.
- **Civilians / Anomalies**.
- Faction relations drive autonomous combat: NPCs sense, chase, take line-of-sight shots, retaliate, and die. Each faction is colour-tinted.

### SCP anomalies (`ASCPEntityBase` + subclasses) with containment
- **SCP-173** — frozen while observed (view cone + line of sight) and not blinking; advances instantly otherwise.
- **SCP-096** — passive until you view its face, then enrages and sprints (inverse of 173).
- **SCP-106** — slow but periodically phases/teleports behind you.
- **SCP-682** — extremely durable, relentlessly aggressive (Keter).
- Every SCP carries **containment metadata**: `ContainmentClass` (Safe/Euclid/Keter/Thaumiel), a `ContainmentProcedure` description, and a required keycard level.
- **`AContainmentTerminal`** (interact with `E`): re-contains the nearest breached SCP if your keycard clearance is high enough — the core "way to contain them" loop.

### Random encounters / open world (`AEncounterManager`)
- Continuously spawns encounters around the player: MTF patrols, Chaos squads, UIU agents, Wondertainment reps, civilian survivors, and **SCP containment breaches**.
- Caps active NPCs, spawns at valid floor points around the player at a min/max radius.

### Objective & loop (`AFoundationGameMode`)
- Find keycards → survive the SCPs and Chaos Insurgency → reach the **Exit Zone** with the required clearance to **escape**.
- Death or escape restarts the level (re-generates a fresh facility).

---

## 5. Tuning (all exposed in the editor)
- `AFacilityGenerator`: `TargetRoomCount`, `RandomSeed`, `SCPCount`, `KeycardCount`, `TerminalCount`, class overrides.
- `AEncounterManager`: `EncounterInterval`, `SpawnRadius`, `MinSpawnDist`, `MaxActiveNPCs`, `SCPPool`.
- `USanityComponent`: blink intervals & duration.
- Each SCP: speed, kill distance, health, containment class/procedure/clearance.

## 6. Extending
- **New SCP**: subclass `ASCPEntityBase`, override `CanMove()` for its containment behaviour, set the metadata in the constructor, and add it to the generator/encounter pools. (e.g. SCP-049, SCP-966, SCP-16829 "The TV in Time" — XI-87's mandate — make great next additions.)
- **New faction**: add to `EFaction` and the relation table in `UFactionLibrary::GetAttitude`.
- **Custom art**: replace the procedural cube geometry in `ARoomBase` / entity meshes with your own modular meshes & materials.

## 7. Source layout
```
Source/Foundation/
  Public/Private/
    FoundationGameMode      — loop, spawns generator + encounter manager
    FoundationCharacter     — FP player, health, keycards, interaction
    SanityComponent         — blink / sanity
    FacilityGenerator       — procedural modular facility
    RoomBase / RoomConnector— modular room pieces
    Faction / FactionCharacter — faction system + NPC AI
    SCPEntityBase / SCP173 / SCP096 / SCP106 / SCP682 — anomalies
    ContainmentTerminal     — re-containment interaction
    DoorActor / KeycardPickup / ExitZone — items & objective
    EncounterManager        — random encounters + MTF XI-87 squad
    InteractableInterface   — E-key interaction contract
```

**Status:** Code complete and self-consistent. Compile inside UE 5.5 to play — the build/editor step cannot be performed in this environment.
