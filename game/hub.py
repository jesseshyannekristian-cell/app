"""Site Overseer Hub menus."""
from . import data, ui, breach
from .archives import Archives


def _pick_index(prompt, count):
    """Ask for a 1-based index or 'b' to go back. Returns 0-based index or None."""
    raw = ui.ask(f"{prompt} (number, or 'b' to go back)", default="b").strip().lower()
    if raw in ("b", ""):
        return None
    if raw.isdigit() and 1 <= int(raw) <= count:
        return int(raw) - 1
    ui.bad("Invalid selection.")
    return None


# ---------------- Research Division ----------------
def research_division(state):
    while True:
        ui.clear()
        ui.header(state)
        t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
        t.add_column("#", style=ui.AMBER, width=4)
        t.add_column("Project")
        t.add_column("Rank", justify="center", width=5)
        t.add_column("Cost", justify="right", width=7)
        t.add_column("Status", justify="center", width=12)
        for i, p in enumerate(data.RESEARCH, 1):
            if state.is_researched(p["id"]):
                status = "[#5dff5d]DONE[/#5dff5d]"
            elif state.rank_level < p["required_rank"]:
                status = "[#ff4b4b]RANK LOCK[/#ff4b4b]"
            elif state.research_credits < p["cost"]:
                status = "[#ffb000]NEED RC[/#ffb000]"
            else:
                status = "[#21d4d4]AVAILABLE[/#21d4d4]"
            t.add_row(str(i), p["name"], str(p["required_rank"]), str(p["cost"]), status)
        ui.console.print(ui.Panel(t, title="RESEARCH DIVISION", border_style=ui.CYAN, box=ui.box.ROUNDED))
        ui.info("Spend Research Credits to complete projects. Each unlocks store gear or a containment procedure.")
        idx = _pick_index("Research which project", len(data.RESEARCH))
        if idx is None:
            return
        p = data.RESEARCH[idx]
        ui.panel(f'{p["objective"]}\n\n[italic]{p["description"]}[/italic]', title=p["name"])
        if ui.confirm(f'Commit {p["cost"]} Research Credits?'):
            ui.result(*state.try_complete_research(p["id"]))
        ui.pause()


# ---------------- Store ----------------
def store(state):
    while True:
        ui.clear()
        ui.header(state)
        t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
        t.add_column("#", style=ui.AMBER, width=4)
        t.add_column("Equipment")
        t.add_column("Dept", width=9)
        t.add_column("Rank", justify="center", width=5)
        t.add_column("Cost", justify="right", width=7)
        t.add_column("Status", justify="center", width=14)
        for i, e in enumerate(data.EQUIPMENT, 1):
            if state.is_owned(e["id"]):
                status = "[#5dff5d]OWNED[/#5dff5d]"
            elif e["requires_research"] and not state.is_equipment_researched(e["id"]):
                status = "[#ffb000]RESEARCH REQ'D[/#ffb000]"
            elif state.rank_level < e["required_rank"]:
                status = "[#ff4b4b]RANK LOCK[/#ff4b4b]"
            else:
                status = "[#21d4d4]BUY[/#21d4d4]"
            t.add_row(str(i), e["name"], e["dept"], str(e["required_rank"]), str(e["cost"]), status)
        ui.console.print(ui.Panel(t, title="REQUISITION STORE", border_style=ui.CYAN, box=ui.box.ROUNDED))
        idx = _pick_index("Purchase which item", len(data.EQUIPMENT))
        if idx is None:
            return
        e = data.EQUIPMENT[idx]
        ui.panel(f'[italic]{e["description"]}[/italic]', title=f'{e["name"]} — {e["cost"]} cr')
        if ui.confirm(f'Purchase {e["name"]} for {e["cost"]} credits?'):
            ui.result(*state.try_purchase(e["id"]))
        ui.pause()


# ---------------- Containment Operations ----------------
def operations(state):
    while True:
        ui.clear()
        ui.header(state)
        t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
        t.add_column("#", style=ui.AMBER, width=4)
        t.add_column("Operation")
        t.add_column("Rank", justify="center", width=5)
        t.add_column("Success", justify="center", width=8)
        t.add_column("Reward (cr/RC/XP)", justify="right", width=18)
        t.add_column("Status", justify="center", width=10)
        for i, o in enumerate(data.OPERATIONS, 1):
            chance = state.operation_success_chance(o["id"])
            reward = f'{o["reward_credits"]}/{o["reward_research"]}/{o["reward_xp"]}'
            if state.is_op_complete(o["id"]):
                status = "[#5dff5d]DONE[/#5dff5d]"
            elif state.rank_level < o["required_rank"]:
                status = "[#ff4b4b]LOCK[/#ff4b4b]"
            else:
                status = "[#21d4d4]READY[/#21d4d4]"
            t.add_row(str(i), o["name"], str(o["required_rank"]), f"{chance}%", reward, status)
        ui.console.print(ui.Panel(t, title="CONTAINMENT OPERATIONS", border_style=ui.CYAN, box=ui.box.ROUNDED))
        ui.info("Dispatch a team. Success is an RNG roll improved by rank; failure still grants field XP.")
        idx = _pick_index("Dispatch which operation", len(data.OPERATIONS))
        if idx is None:
            return
        o = data.OPERATIONS[idx]
        reqs = []
        if o["required_research"]:
            reqs.append(f'Procedure: {o["required_research"]}')
        if o["required_equipment"]:
            req = data.find_equipment(o["required_equipment"])
            reqs.append(f'Equipment: {req["name"] if req else o["required_equipment"]}')
        body = f'[bold]{o["target"]}[/bold]\n\n{o["description"]}'
        if reqs:
            body += "\n\n[italic]Requires:[/italic] " + ", ".join(reqs)
        ui.panel(body, title=o["name"])
        if ui.confirm(f'Dispatch team? ({state.operation_success_chance(o["id"])}% success)'):
            ui.result(*state.try_run_operation(o["id"]))
        ui.pause()


# ---------------- Loadout ----------------
def loadout(state):
    while True:
        ui.clear()
        ui.header(state)
        owned = [e for e in data.EQUIPMENT if state.is_owned(e["id"])]
        if not owned:
            ui.panel("You own no equipment. Visit the Store first.", title="PRE-BREACH LOADOUT", style=ui.RED)
            ui.pause()
            return
        t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
        t.add_column("#", style=ui.AMBER, width=4)
        t.add_column("Owned Equipment")
        t.add_column("In Loadout", justify="center", width=12)
        for i, e in enumerate(owned, 1):
            mark = "[#5dff5d]● EQUIPPED[/#5dff5d]" if state.is_in_loadout(e["id"]) else "[#7a7a7a]—[/#7a7a7a]"
            t.add_row(str(i), e["name"], mark)
        cap = f"{len(state.loadout)}/4 slots used"
        ui.console.print(ui.Panel(t, title=f"PRE-BREACH LOADOUT  ({cap})", border_style=ui.CYAN, box=ui.box.ROUNDED))
        ui.info("Equipped gear boosts your survival chance during a breach.")
        idx = _pick_index("Toggle which item", len(owned))
        if idx is None:
            return
        ui.result(*state.toggle_loadout(owned[idx]["id"]))
        ui.pause()


# ---------------- Breach ----------------
def breach_menu(state, archives):
    ui.clear()
    ui.header(state)
    targets = [dict(number=s["number"], name=s["name"], threat=s["threat"]) for s in data.SCPS if s["breachable"]]
    for c in archives.breachable():
        targets.append(dict(number=c["designation"], name=c["name"], threat=3, custom=True))
    t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
    t.add_column("#", style=ui.AMBER, width=4)
    t.add_column("Anomaly")
    t.add_column("Threat", justify="center", width=8)
    t.add_column("Source", justify="center", width=10)
    for i, tg in enumerate(targets, 1):
        src = "[#ffb000]CUSTOM[/#ffb000]" if tg.get("custom") else "Site-20"
        t.add_row(str(i), f'{tg["number"]} — {tg["name"]}', "▲" * tg["threat"], src)
    ui.console.print(ui.Panel(t, title="BREACH — SELECT ANOMALY TO RESPOND TO", border_style="bold #ff4b4b", box=ui.box.ROUNDED))
    rec = f'Survived {state.breaches_survived}  ·  Failed {state.breaches_failed}  ·  Re-contained {state.scps_recontained}'
    ui.info("Live-breach record — " + rec)
    idx = _pick_index("Respond to which breach", len(targets))
    if idx is None:
        return
    breach.run_breach(state, targets[idx])
    ui.pause()


# ---------------- Personnel ----------------
def personnel(state):
    while True:
        ui.clear()
        ui.header(state)
        t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
        t.add_column("#", style=ui.AMBER, width=4)
        t.add_column("Codename")
        t.add_column("Position")
        t.add_column("Clearance", width=22)
        for i, p in enumerate(data.PERSONNEL, 1):
            t.add_row(str(i), p["codename"], p["position"], p["clearance"])
        ui.console.print(ui.Panel(t, title="SITE-20 PERSONNEL", border_style=ui.CYAN, box=ui.box.ROUNDED))
        idx = _pick_index("Open which dossier", len(data.PERSONNEL))
        if idx is None:
            return
        p = data.PERSONNEL[idx]
        body = (
            f'[bold #ffb000]{p["codename"]}[/bold #ffb000]   ·   File {p["file_no"]}\n'
            f'[#7a7a7a]Real name:[/#7a7a7a] {p["real_name"]}\n'
            f'[#7a7a7a]Position:[/#7a7a7a] {p["position"]}\n'
            f'[#7a7a7a]Clearance:[/#7a7a7a] {p["clearance"]}\n'
            f'[#7a7a7a]Status:[/#7a7a7a] {p["status"]}\n'
            f'[#7a7a7a]Nationality:[/#7a7a7a] {p["nationality"]}\n'
            f'[#7a7a7a]Affiliation:[/#7a7a7a] {p["affiliation"]}\n'
            f'[#7a7a7a]Aliases:[/#7a7a7a] {p["aliases"]}\n'
            f'[#7a7a7a]Anomalous trait:[/#7a7a7a] {p["anomalous_trait"]}\n\n'
            f'{p["bio"]}\n\n'
            f'[italic #21d4d4]{p["o5_notes"]}[/italic #21d4d4]'
        )
        ui.panel(body, title=f'DOSSIER — {p["codename"]}')
        ui.pause()


# ---------------- Ranks & Rewards ----------------
def ranks(state):
    ui.clear()
    ui.header(state)
    t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
    t.add_column("Lvl", style=ui.AMBER, width=4)
    t.add_column("Title")
    t.add_column("XP Req", justify="right", width=8)
    t.add_column("Promotion Reward (cr/RC)", justify="right", width=24)
    t.add_column("", width=8)
    for r in data.RANKS:
        marker = "[#5dff5d]◀ YOU[/#5dff5d]" if r["level"] == state.rank_level else ""
        t.add_row(str(r["level"]), r["title"], str(r["xp_required"]),
                  f'{r["credit_reward"]}/{r["research_credit_reward"]}', marker)
    ui.console.print(ui.Panel(t, title="RANKS & REWARDS", border_style=ui.CYAN, box=ui.box.ROUNDED))
    if state.has_next_rank():
        nxt = state.next_rank()
        need = nxt["xp_required"] - state.xp
        ui.info(f'Next: {nxt["title"]} — {need} XP to promotion.')
    else:
        ui.good("Maximum rank achieved: O5 Overseer (OMEGA BLACK).")
    breach_body = (f'Breaches survived: {state.breaches_survived}\n'
                   f'Breaches failed: {state.breaches_failed}\n'
                   f'SCPs re-contained: {state.scps_recontained}')
    ui.panel(breach_body, title="LIVE-BREACH RECORD", style=ui.AMBER)
    ui.pause()


# ---------------- Archives ----------------
_CLASSES = ["Safe", "Euclid", "Keter", "Thaumiel", "Apollyon", "Neutralized"]


def archives_menu(state, archives):
    while True:
        ui.clear()
        ui.header(state)
        if archives.custom_scps:
            t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
            t.add_column("#", style=ui.AMBER, width=4)
            t.add_column("Designation", width=14)
            t.add_column("Name")
            t.add_column("Class", width=12)
            t.add_column("Breachable", justify="center", width=11)
            for i, s in enumerate(archives.custom_scps, 1):
                br = "[#5dff5d]YES[/#5dff5d]" if s.get("is_breachable") else "[#7a7a7a]no[/#7a7a7a]"
                t.add_row(str(i), s["designation"], s["name"], s["class"], br)
            ui.console.print(ui.Panel(t, title="ARCHIVES — CUSTOM SCP DATABASE", border_style=ui.CYAN, box=ui.box.ROUNDED))
        else:
            ui.panel("No custom SCPs filed yet.", title="ARCHIVES — CUSTOM SCP DATABASE")
        choice = ui.menu("ARCHIVE ACTIONS", [
            ("n", "File a new SCP"),
            ("v", "View an SCP report"),
            ("e", "Edit an SCP"),
            ("d", "Delete an SCP"),
            ("b", "Back to hub"),
        ])
        if choice == "b":
            return
        if choice == "n":
            _archive_create(archives)
        elif choice == "v":
            _archive_view(archives)
        elif choice == "e":
            _archive_edit(archives)
        elif choice == "d":
            _archive_delete(archives)


def _archive_create(archives):
    ui.panel("Filing a new anomalous object report.", title="NEW SCP")
    designation = ui.ask("Designation (e.g. SCP-7777)") or "SCP-XXXX"
    name = ui.ask("Name / nickname") or "Unknown"
    obj_class = ui.menu("Object class", [(c[0].lower(), c) for c in _CLASSES])
    obj_class = next(c for c in _CLASSES if c[0].lower() == obj_class)
    description = ui.ask("Description / containment notes") or ""
    breachable = ui.confirm("Mark as breachable (can appear in the Breach menu)?")
    entry = archives.add(designation, name, obj_class, description, breachable)
    ui.good(f'Filed {entry["designation"]} — {entry["name"]}.')
    ui.pause()


def _archive_view(archives):
    if not archives.custom_scps:
        ui.bad("Nothing to view.")
        ui.pause()
        return
    idx = _pick_index("View which entry", len(archives.custom_scps))
    if idx is None:
        return
    s = archives.custom_scps[idx]
    body = (f'[bold #ffb000]{s["designation"]}[/bold #ffb000] — {s["name"]}\n'
            f'[#7a7a7a]Object Class:[/#7a7a7a] {s["class"]}\n'
            f'[#7a7a7a]Breachable:[/#7a7a7a] {"Yes" if s.get("is_breachable") else "No"}\n\n'
            f'{s["description"]}')
    ui.panel(body, title=f'ARCHIVE FILE #{s["id"]}')
    ui.pause()


def _archive_edit(archives):
    if not archives.custom_scps:
        ui.bad("Nothing to edit.")
        ui.pause()
        return
    idx = _pick_index("Edit which entry", len(archives.custom_scps))
    if idx is None:
        return
    s = archives.custom_scps[idx]
    name = ui.ask("Name", default=s["name"])
    description = ui.ask("Description", default=s["description"])
    breachable = ui.confirm("Breachable?")
    archives.update(s["id"], name=name, description=description, is_breachable=breachable)
    ui.good("Entry updated.")
    ui.pause()


def _archive_delete(archives):
    if not archives.custom_scps:
        ui.bad("Nothing to delete.")
        ui.pause()
        return
    idx = _pick_index("Delete which entry", len(archives.custom_scps))
    if idx is None:
        return
    s = archives.custom_scps[idx]
    if ui.confirm(f'Permanently delete {s["designation"]} — {s["name"]}?'):
        archives.delete(s["id"])
        ui.good("Entry deleted.")
    ui.pause()
