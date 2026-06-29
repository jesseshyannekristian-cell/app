"""Text-based containment breach — branching mini-scenarios + RNG survival, with Iron-man support."""
import string

from . import art, data, rng, scenarios, ui

# Survival-chance modifier applied per difficulty tier.
DIFFICULTY_MOD = {"Easy": 15, "Normal": 0, "Hard": -15}

# Loadout items that meaningfully improve breach survival, with flavour.
LOADOUT_BONUS = {
    "kevlar_vest": (8, "Kevlar vest absorbs a glancing blow."),
    "epsilon_rifle": (12, "Epsilon-11 rifle lays down covering fire."),
    "scranton_anchor": (15, "Scranton Anchor stabilises reality around the corridor."),
    "micro_hid": (18, "Micro-H.I.D. discharge forces the anomaly back."),
    "hazmat_suit": (7, "Hazmat suit shrugs off corrosive splatter."),
    "thermal_optics": (9, "Thermal optics reveal the anomaly in the dark."),
    "femur_breaker": (10, "Femur-bone lure draws the entity toward the recall trap."),
    "flashlight": (4, "Your flashlight keeps the dark at bay."),
    "keycard_l1": (3, "Keycard lets you seal a blast door behind you."),
}

GENERIC_EVENTS = [
    "Containment alarms wail as red emergency lighting floods Site-20.",
    "A blast door grinds shut somewhere down the corridor.",
    "Static crackles over the PA. Someone is screaming, then silence.",
    "You move through the dark, boots echoing on wet concrete.",
    "The anomaly's signature spikes on the Kant Counter.",
]

_SCP_FLAVOR = {
    "SCP-173": "Do NOT blink. It moves the instant you look away.",
    "SCP-096": "Keep your eyes off its face — or it will run you down.",
    "SCP-106": "The walls weep corrosion. The Old Man is phasing closer.",
    "SCP-682": "Acid hisses where it walks. Nothing has ever killed it.",
    "SCP-049": "The Plague Doctor approaches, murmuring about the cure.",
    "SCP-16829": "An old television flickers — the timeline starts to loop.",
}


def _assess(state, threat):
    """Return (chance, base, rank_bonus, loadout_bonus, diff_mod, bonus_lines)."""
    loadout_bonus = 0
    bonus_lines = []
    for item in state.loadout:
        b = LOADOUT_BONUS.get(item)
        if not b:
            continue
        loadout_bonus += b[0]
        equip = data.find_equipment(item)
        bonus_lines.append(f'+{b[0]}%  {equip["name"] if equip else item}')
    base = 60 - threat * 8
    rank_bonus = state.rank_level * 5
    diff_mod = DIFFICULTY_MOD.get(getattr(state, "difficulty", "Normal"), 0)
    chance = max(10, min(95, base + rank_bonus + loadout_bonus + diff_mod))
    return chance, base, rank_bonus, loadout_bonus, diff_mod, bonus_lines


def _render_assessment(state, base, rank_bonus, loadout_bonus, diff_mod, chance, bonus_lines, has_loadout):
    t = ui.Table(box=ui.box.SIMPLE, show_header=False)
    t.add_column(style=ui.DIM)
    t.add_column(justify="right", style=ui.CYAN)
    t.add_row("Base survival (threat lvl)", f"{base}%")
    t.add_row("Rank modifier", f"+{rank_bonus}%")
    t.add_row("Loadout modifier", f"+{loadout_bonus}%")
    t.add_row(f'Difficulty ({getattr(state, "difficulty", "Normal")})', f"{diff_mod:+d}%")
    t.add_row("[bold]SURVIVAL CHANCE[/bold]", f"[bold]{chance}%[/bold]")
    ui.console.print(ui.Panel(t, title="PRE-BREACH ASSESSMENT", border_style=ui.AMBER, box=ui.box.ROUNDED))
    if has_loadout:
        ui.info("Loadout active: " + "  ·  ".join(bonus_lines))
    else:
        ui.bad("No loadout equipped — you go in with nothing but nerve.")


def _next_log_line(state):
    if state.loadout and rng.chance(60):
        b = LOADOUT_BONUS.get(rng.choice(state.loadout))
        if b:
            return b[1]
    return rng.choice(GENERIC_EVENTS)


def _simulate(state):
    """Generic breach log for anomalies without a scripted scenario."""
    for _ in range(rng.roll(3, 5)):
        ui.slow_print(_next_log_line(state))


def _branching(state, number, chance):
    """Run the scripted decision stages; return the adjusted survival chance."""
    opt_keys = [c for c in string.ascii_lowercase if c != "x"]
    for stage in scenarios.stages(number):
        options = stage["options"]
        if len(options) > len(opt_keys):
            raise ValueError(f"scenario {number} stage has too many options ({len(options)})")
        keys = opt_keys[:len(options)]
        ui.panel(stage["prompt"], title="DECISION", style=ui.AMBER)
        opts = [(keys[i], label) for i, (label, _, _) in enumerate(options)]
        choice = ui.menu("Your call, Overseer", opts + [("x", "Improvise (50/50)")])
        if choice == "x":
            delta, flavor = (8, "You improvise. It works — barely.") if rng.chance(50) \
                else (-12, "You improvise. It does not go to plan.")
        else:
            _, delta, flavor = options[keys.index(choice)]
        chance = max(5, min(95, chance + delta))
        ui.slow_print(flavor, style=(ui.GREEN if delta >= 0 else ui.RED))
        ui.info(f"Survival chance now {chance}%")
    return chance


def _resolve(state, number, chance):
    """Resolve the final roll; record result. Return True if survived."""
    survived = rng.roll(1, 100) <= chance
    if survived:
        ui.panel(
            f"Recontainment successful. {number} is back behind its door.\n"
            "Hazard pay and research data logged.",
            title="✔ BREACH CONTAINED", style=ui.GREEN,
        )
        state.record_breach_result(True, recontained=1)
    else:
        ui.panel(
            f"{number} overwhelmed the response team. The breach spreads.\n"
            "You barely make it to the evac point. Failure logged.",
            title="✘ CONTAINMENT FAILED", style=ui.RED,
        )
        state.record_breach_result(False)
    ui.info(f"Record — Survived: {state.breaches_survived}  Failed: {state.breaches_failed}  "
            f"Re-contained: {state.scps_recontained}")
    return survived


def run_breach(state, target):
    """Run a breach. Returns True (survived), False (failed), or None (aborted / iron-man wipe)."""
    number = target.get("number") or target.get("designation")
    name = target.get("name", "")
    threat = target.get("threat", 3)

    ui.clear()
    ui.art(art.for_scp(number))
    ui.panel(
        f'[bold]{number} — {name}[/bold]\n\n{_SCP_FLAVOR.get(number, "The anomaly is loose in the corridors.")}',
        title="⚠ CONTAINMENT BREACH ⚠", style="bold #ff4b4b",
    )

    chance, base, rank_bonus, loadout_bonus, diff_mod, bonus_lines = _assess(state, threat)
    _render_assessment(state, base, rank_bonus, loadout_bonus, diff_mod, chance, bonus_lines, bool(bonus_lines))

    if not ui.confirm("\nDeploy into the breach?"):
        ui.info("Deployment aborted. The cell holds — for now.")
        return None

    ui.console.print()
    if scenarios.has(number):
        chance = _branching(state, number, chance)
    else:
        _simulate(state)
    ui.console.print()

    survived = _resolve(state, number, chance)

    if not survived and getattr(state, "ironman", False):
        ui.panel(
            "IRON-MAN PROTOCOL: a containment failure ends your career. "
            "Your save has been wiped. The Foundation endures without you.",
            title="☠ PERMADEATH", style="bold #ff4b4b",
        )
        state.new_game(ironman=True, difficulty=getattr(state, "difficulty", "Normal"))
        return None
    return survived
