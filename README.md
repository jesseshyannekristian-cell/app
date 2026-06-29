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
- **SCP-16829 "The TV in Time"** (`ASCP16829`) — stationary anomalous television (Dimension Chemosic).
  Interact (E) for dimensional travel to a random room; lingering in its field triggers a temporal
  rewind (time loop); prolonged exposure drains sanity and is carcinogenic. Spawned once per facility
  by the generator (`SignatureAnomalyClass`).
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

---

## 8. Title screen & Site Overseer meta-game (NEW)

The game now boots into a **title screen** ("SCP SITE OVERSEER") built entirely in C++/Slate.
The provided artwork (`Content/UI/TitleScreen.png`) is loaded at runtime and shown as a
letterboxed backdrop, with real clickable buttons placed over the painted START GAME / OPTIONS / CREDITS.

- **Boot flow:** `MainMenuGameMode` is the default GameMode → `MainMenuPlayerController` shows the
  title (`SMainMenuWidget`).
- **START GAME** → opens the **Site Overseer Hub** (`SSiteOverseerHub`).
- **OPTIONS / CREDITS** → in-menu panels (Credits shows full CC BY-SA 3.0 attribution). Options has Quit.
- The **BREACH** tab re-opens the map with `?Game=FoundationGameMode` to launch the existing FPS gameplay.
  On death/escape the player returns to the hub; rewards are recorded.

### Site Overseer Hub — tabs
- **RESEARCH DIVISION** — research objectives + projects. Spend **Research Credits** to complete a
  project; each unlocks either a level-locked store item or a **containment procedure**.
- **STORE** — purchase equipment with **Credits**. Level-locked items show `RESEARCH REQ'D` until
  their project is complete (and rank met).
- **CONTAINMENT OPERATIONS** — menu-driven ops to contain every SCP and neutralise every rogue
  MTF / GOI threat. Gated by rank, required equipment, and (for key SCPs) a researched procedure.
  Dispatch grants Credits / Research Credits / XP.
- **BREACH** — lists all breachable SCPs and shows the live-breach record; DEPLOY launches the FPS level.
- **SITE-20 PERSONNEL** — lore dossiers for key staff (GUNZ — O5 Council/"The Technician"; CHEMOSIC —
  Site Supervisor & former MTF XI-87 Commander; Head Researcher) with codename, clearance, affiliation,
  anomalous traits (SCP-16829 exposure) and O5 notes. UI/lore only.
- **RANKS & REWARDS** — rank ladder (Junior Researcher → O5 Overseer), promotion rewards, and a
  **breach section** (survived / failed / re-contained).

### Economy & persistence
- Two currencies: **Credits** (gear) and **Research Credits** (research only).
- `UOverseerProgression` (GameInstanceSubsystem) persists everything to a `UOverseerSaveGame`
  ("OverseerSave" slot): currencies, XP/rank, completed research, owned equipment, completed ops,
  breach record. Rank-ups grant bonus currency automatically.

### Data
All rosters live in `UFoundationDataLibrary` (code-defined, easy to extend):
SCP roster (173/096/106/682/049/035/939/966/999/053/079/087/457/1048/1471/2521/3008/4666/008/914/055),
rogue MTF / GOI threats (Chaos Insurgency, defected Nine-Tailed Fox, Serpent's Hand, rogue Alpha-1,
GOC, MC&D, defected Tau-5, sleeper agents), field & research equipment, ranks, and auto-generated
operations + per-item equipment research projects.

> **Packaging note:** `Content/UI/TitleScreen.png` is loaded from disk at runtime (works in editor/PIE).
> For a packaged build, either import it as a Texture2D asset or add `Content/UI` under
> *Project Settings → Packaging → Additional Non-Asset Directories to Package*.

---

## 9. License & Attribution

This project's **source code** and the **SCP-themed content** it references are released under the
**Creative Commons Attribution-ShareAlike 3.0 Unported License (CC BY-SA 3.0)**.

> You are free to **share** (copy and redistribute) and **adapt** (remix, transform, build upon) this
> material for any purpose, even commercially, under the following terms:
> - **Attribution** — You must give appropriate credit, provide a link to the license, and indicate if
>   changes were made.
> - **ShareAlike** — If you remix, transform, or build upon the material, you must distribute your
>   contributions under the same license as the original.
>
> Full license text: https://creativecommons.org/licenses/by-sa/3.0/

### SCP Foundation content
All SCP names, numbers, descriptions, and lore (e.g. SCP-173, SCP-096, SCP-106, SCP-682, MTF XI-87,
Chaos Insurgency, UIU, Dr. Wondertainment) are the creative work of the **SCP Foundation community**
and are likewise licensed under **CC BY-SA 3.0**.

- SCP Foundation Wiki: https://scp-wiki.wikidot.com/
- Licensing guide: https://scp-wiki.wikidot.com/licensing-guide

This game is a non-official, fan-made work. It is not affiliated with or endorsed by the SCP Foundation
or any of its authors. Per the ShareAlike requirement, any derivative of this project must remain under
CC BY-SA 3.0.

> **Note:** Unreal Engine itself is governed by the separate **Epic Games / Unreal Engine EULA**, which
> applies to the engine and is independent of the CC BY-SA 3.0 license covering this project's content.

### Applicable legal notices
| Component | License / Terms | Reference |
|---|---|---|
| This project's source code & content | CC BY-SA 3.0 | https://creativecommons.org/licenses/by-sa/3.0/ |
| SCP Foundation lore, names & numbers | CC BY-SA 3.0 | https://scp-wiki.wikidot.com/licensing-guide |
| Unreal Engine (engine, headers, runtime) | Unreal Engine EULA | https://www.unrealengine.com/eula |
| Engine BasicShapes mesh (`Cube.Cube`) used for geometry | Unreal Engine EULA (Epic-provided content) | https://www.unrealengine.com/eula |

### Trademarks
"SCP Foundation", "Unreal", and "Unreal Engine" are trademarks or registered trademarks of their
respective owners. Use here is nominative/descriptive only and does not imply affiliation or endorsement.

### Disclaimer of warranty
This software is provided **"as is"**, without warranty of any kind, express or implied. To the maximum
extent permitted by law, the authors are not liable for any claim, damages, or other liability arising
from the use of this software.

See [`LICENSE`](LICENSE) for the full CC BY-SA 3.0 text and [`CREDITS.md`](CREDITS.md) for full
attribution.
