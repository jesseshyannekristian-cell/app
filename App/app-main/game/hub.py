"""Site Overseer Hub menus."""
from . import data, ui, breach, achievements
from .archives import Archives


def _notify(state, archives=None, ctx=None):
    for a in achievements.check(state, archives, ctx):
        ui.good(f'ACHIEVEMENT UNLOCKED — {a["name"]}: {a["desc"]}')


def _pick_index(prompt, count):
    """Ask for a 1-based index or 'b' to go back. Returns 0-based index or None."""
    raw = ui.ask(f"{prompt} (number, or 'b' to go back)", default="b").strip().lower()
    if raw in ("b", ""):
        return None
    if raw.isdigit() and 1 <= int(raw) <= count:
        return int(raw) - 1
    ui.bad("Invalid selection.")
    return None


# ---- status-cell helpers (keep render loops flat) ----
def _research_status(state, p):
    if state.is_researched(p["id"]):
        return "[#5dff5d]DONE[/#5dff5d]"
    if state.rank_level < p["required_rank"]:
        return "[#ff4b4b]RANK LOCK[/#ff4b4b]"
    if state.research_credits < p["cost"]:
        return "[#ffb000]NEED RC[/#ffb000]"
    return "[#21d4d4]AVAILABLE[/#21d4d4]"


def _store_status(state, e):
    if state.is_owned(e["id"]):
        return "[#5dff5d]OWNED[/#5dff5d]"
    if e["requires_research"] and not state.is_equipment_researched(e["id"]):
        return "[#ffb000]RESEARCH REQ'D[/#ffb000]"
    if state.rank_level < e["required_rank"]:
        return "[#ff4b4b]RANK LOCK[/#ff4b4b]"
    return "[#21d4d4]BUY[/#21d4d4]"


def _op_status(state, o):
    if state.is_op_complete(o["id"]):
        return "[#5dff5d]DONE[/#5dff5d]"
    if state.rank_level < o["required_rank"]:
        return "[#ff4b4b]LOCK[/#ff4b4b]"
    return "[#21d4d4]READY[/#21d4d4]"


def _op_detail_body(o):
    reqs = []
    if o["required_research"]:
        reqs.append(f'Procedure: {o["required_research"]}')
    if o["required_equipment"]:
        req = data.find_equipment(o["required_equipment"])
        reqs.append(f'Equipment: {req["name"] if req else o["required_equipment"]}')
    body = f'[bold]{o["target"]}[/bold]\n\n{o["description"]}'
    if reqs:
        body += "\n\n[italic]Requires:[/italic] " + ", ".join(reqs)
    return body


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
            t.add_row(str(i), p["name"], str(p["required_rank"]), str(p["cost"]), _research_status(state, p))
        ui.console.print(ui.Panel(t, title="RESEARCH DIVISION", border_style=ui.CYAN, box=ui.box.ROUNDED))
        ui.info("Spend Research Credits to complete projects. Each unlocks store gear or a containment procedure.")
        idx = _pick_index("Research which project", len(data.RESEARCH))
        if idx is None:
            return
        p = data.RESEARCH[idx]
        ui.panel(f'{p["objective"]}\n\n[italic]{p["description"]}[/italic]', title=p["name"])
        if ui.confirm(f'Commit {p["cost"]} Research Credits?'):
            ui.result(*state.try_complete_research(p["id"]))
            _notify(state)
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
            t.add_row(str(i), e["name"], e["dept"], str(e["required_rank"]), str(e["cost"]), _store_status(state, e))
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
            t.add_row(str(i), o["name"], str(o["required_rank"]), f"{chance}%", reward, _op_status(state, o))
        ui.console.print(ui.Panel(t, title="CONTAINMENT OPERATIONS", border_style=ui.CYAN, box=ui.box.ROUNDED))
        ui.info("Dispatch a team. Success is an RNG roll improved by rank; failure still grants field XP.")
        idx = _pick_index("Dispatch which operation", len(data.OPERATIONS))
        if idx is None:
            return
        o = data.OPERATIONS[idx]
        ui.panel(_op_detail_body(o), title=o["name"])
        if ui.confirm(f'Dispatch team? ({state.operation_success_chance(o["id"])}% success)'):
            ui.result(*state.try_run_operation(o["id"]))
            _notify(state)
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
        _notify(state)
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
    had_loadout = bool(state.loadout)
    survived = breach.run_breach(state, targets[idx])
    if survived is not None:
        _notify(state, archives, ctx={"breach_win_no_loadout": survived and not had_loadout})
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


# ---------------- Achievements ----------------
def achievements_menu(state):
    ui.clear()
    ui.header(state)
    t = ui.Table(box=ui.box.SIMPLE_HEAD, expand=True)
    t.add_column("", width=3)
    t.add_column("Achievement")
    t.add_column("Requirement")
    for a in achievements.ACHIEVEMENTS:
        done = a["id"] in state.unlocked_achievements
        mark = "[#5dff5d]★[/#5dff5d]" if done else "[#7a7a7a]☆[/#7a7a7a]"
        name = f'[#5dff5d]{a["name"]}[/#5dff5d]' if done else f'[#7a7a7a]{a["name"]}[/#7a7a7a]'
        t.add_row(mark, name, a["desc"])
    unlocked = len(state.unlocked_achievements)
    ui.console.print(ui.Panel(t, title=f"ACHIEVEMENTS  ({unlocked}/{len(achievements.ACHIEVEMENTS)})",
                              border_style=ui.CYAN, box=ui.box.ROUNDED))
    ui.pause()


# ---------------- Site Status Briefing ----------------
def _xp_bar(state, width=24):
    if not state.has_next_rank():
        return "█" * width + f"  {state.xp} XP · MAX RANK"
    cur, nxt = state.current_rank(), state.next_rank()
    lo, hi = cur["xp_required"], nxt["xp_required"]
    frac = max(0.0, min(1.0, (state.xp - lo) / (hi - lo))) if hi > lo else 1.0
    filled = int(frac * width)
    return "█" * filled + "░" * (width - filled) + f"  {state.xp}/{hi} XP"


def _next_objective(state):
    if not state.owned_equipment:
        return "Visit the Requisition Store and buy your first piece of equipment."
    avail_res = [p for p in data.RESEARCH if not state.is_researched(p["id"])
                 and state.rank_level >= p["required_rank"] and state.research_credits >= p["cost"]]
    if avail_res:
        return f'Research is ready to authorise: {avail_res[0]["name"]}.'
    avail_op = [o for o in data.OPERATIONS if not state.is_op_complete(o["id"])
                and state.rank_level >= o["required_rank"]]
    if state.has_next_rank() and avail_op:
        return f'Dispatch containment operations to earn XP toward {state.next_rank()["title"]}.'
    if not state.loadout:
        return "Equip a pre-breach loadout, then respond to a live breach."
    return "Respond to a breach to contain a live anomaly and bank hazard pay."


def site_status(state, archives, clear=True):
    if clear:
        ui.clear()
        ui.header(state)
    rank = state.current_rank()
    iron = "  ·  [#ff4b4b]☠ IRON-MAN[/#ff4b4b]" if getattr(state, "ironman", False) else ""
    ach = len(state.unlocked_achievements)
    body = (
        f'[bold #ffb000]Welcome back, {rank["title"]}.[/bold #ffb000]\n\n'
        f'[#7a7a7a]Rank / Clearance:[/#7a7a7a]  {rank["title"]} (Level {rank["level"]})\n'
        f'[#7a7a7a]XP Progress:[/#7a7a7a]      {_xp_bar(state)}\n'
        f'[#7a7a7a]Difficulty:[/#7a7a7a]       {getattr(state, "difficulty", "Normal")}{iron}\n\n'
        f'[#7a7a7a]Operations contained:[/#7a7a7a]   {len(state.completed_ops)}/{len(data.OPERATIONS)}\n'
        f'[#7a7a7a]Breaches survived/failed:[/#7a7a7a] {state.breaches_survived} / {state.breaches_failed}\n'
        f'[#7a7a7a]SCPs re-contained:[/#7a7a7a]      {state.scps_recontained}\n'
        f'[#7a7a7a]Custom SCPs filed:[/#7a7a7a]      {len(archives.custom_scps)}\n'
        f'[#7a7a7a]Research completed:[/#7a7a7a]     {len(state.completed_research)}\n'
        f'[#7a7a7a]Achievements unlocked:[/#7a7a7a]  {ach}/{len(achievements.ACHIEVEMENTS)}'
    )
    ui.panel(body, title="◆ SITE-20 STATUS BRIEFING ◆", style=ui.AMBER)
    ui.panel(f'[bold]Recommended next action:[/bold]\n{_next_objective(state)}',
             title="DIRECTIVE — O5 COMMAND", style=ui.CYAN)
    ui.pause()


# ---------------- New Game / Reset ----------------
_DIFFICULTIES = [
    ("e", "Easy", "+15% breach survival — for a relaxed playthrough."),
    ("n", "Normal", "Balanced — the standard Site-20 experience."),
    ("h", "Hard", "-15% breach survival — for seasoned Overseers."),
]


def new_game_menu(state, archives):
    ui.clear()
    ui.header(state)
    ui.panel(
        "Start a NEW GAME. This wipes your progression (rank, currencies, research, equipment, "
        "breach record and achievements).\n\n"
        "Choose a difficulty tier, then confirm the summary below.",
        title="NEW GAME / RESET", style=ui.RED,
    )
    # Gather all choices first, then a single final confirmation.
    t = ui.Table(box=ui.box.SIMPLE, show_header=False, padding=(0, 2))
    t.add_column(style=ui.AMBER)
    t.add_column()
    for key, name, desc in _DIFFICULTIES:
        t.add_row(name, desc)
    ui.console.print(ui.Panel(t, title="DIFFICULTY TIERS", border_style=ui.CYAN, box=ui.box.ROUNDED))
    dkey = ui.menu("Select difficulty", [(k, n) for k, n, _ in _DIFFICULTIES])
    difficulty = next(n for k, n, _ in _DIFFICULTIES if k == dkey)
    ironman = ui.confirm("Enable Iron-man mode (a failed breach permanently wipes your save)?")
    wipe_archives = ui.confirm("Also delete all custom SCPs from the Archives?")

    plan = (
        f'[#7a7a7a]Difficulty:[/#7a7a7a] {difficulty}\n'
        f'[#7a7a7a]Iron-man:[/#7a7a7a] {"ON — permadeath" if ironman else "off"}\n'
        f'[#7a7a7a]Archives:[/#7a7a7a] '
        f'{f"DELETE all {len(archives.custom_scps)} custom SCP(s)" if wipe_archives else "keep"}\n\n'
        '[bold #ff4b4b]This will erase your current progression and cannot be undone.[/bold #ff4b4b]'
    )
    ui.panel(plan, title="CONFIRM NEW GAME", style=ui.AMBER)
    if not ui.confirm("Proceed with these settings?"):
        ui.info("Cancelled. Your career continues.")
        ui.pause()
        return
    state.new_game(ironman=ironman, difficulty=difficulty)
    if wipe_archives:
        archives.custom_scps = []
        archives.save()
    ui.good(f'New game started — {difficulty}{" · IRON-MAN" if ironman else ""}.')
    ui.pause()


# ---------------- Archives ----------------
_CLASSES = ["Safe", "Euclid", "Keter", "Thaumiel", "Apollyon", "Neutralized"]


def _render_archive_table(archives):
    if not archives.custom_scps:
        ui.panel("No custom SCPs filed yet.", title="ARCHIVES — CUSTOM SCP DATABASE")
        return
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


def archives_menu(state, archives):
    while True:
        ui.clear()
        ui.header(state)
        _render_archive_table(archives)
        choice = ui.menu("ARCHIVE ACTIONS", [
            ("n", "File a new SCP"),
            ("v", "View an SCP report"),
            ("e", "Edit an SCP"),
            ("d", "Delete an SCP"),
            ("b", "Back to hub"),
        ])
        if choice == "b":
            return
        _ARCHIVE_ACTIONS[choice](archives)
        _notify(state, archives)



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



_ARCHIVE_ACTIONS = {
    "n": _archive_create,
    "v": _archive_view,
    "e": _archive_edit,
    "d": _archive_delete,
}
