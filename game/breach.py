"""Text-based containment breach — RNG survival event boosted by pre-breach loadout."""
import random

from . import data, ui

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


def _scp_flavor(number):
    flavor = {
        "SCP-173": "Do NOT blink. It moves the instant you look away.",
        "SCP-096": "Keep your eyes off its face — or it will run you down.",
        "SCP-106": "The walls weep corrosion. The Old Man is phasing closer.",
        "SCP-682": "Acid hisses where it walks. Nothing has ever killed it.",
        "SCP-049": "The Plague Doctor approaches, murmuring about the cure.",
        "SCP-16829": "An old television flickers — the timeline starts to loop.",
    }
    return flavor.get(number, "The anomaly is loose in the corridors.")


def run_breach(state, target):
    """target: dict with at least 'number'/'designation', 'name', 'threat'."""
    number = target.get("number") or target.get("designation")
    name = target.get("name", "")
    threat = target.get("threat", 3)

    ui.clear()
    ui.panel(
        f"[bold]{number} — {name}[/bold]\n\n{_scp_flavor(number)}",
        title="⚠ CONTAINMENT BREACH ⚠", style="bold #ff4b4b",
    )

    # Compute survival chance from threat, rank and loadout.
    bonus_lines = []
    loadout_bonus = 0
    for item in state.loadout:
        b = LOADOUT_BONUS.get(item)
        if b:
            loadout_bonus += b[0]
            bonus_lines.append(f'+{b[0]}%  {data.find_equipment(item)["name"] if data.find_equipment(item) else item}')

    base = 60 - threat * 8
    rank_bonus = state.rank_level * 5
    chance = max(10, min(95, base + rank_bonus + loadout_bonus))

    t = ui.Table(box=ui.box.SIMPLE, show_header=False)
    t.add_column(style=ui.DIM)
    t.add_column(justify="right", style=ui.CYAN)
    t.add_row("Base survival (threat lvl)", f"{base}%")
    t.add_row("Rank modifier", f"+{rank_bonus}%")
    t.add_row("Loadout modifier", f"+{loadout_bonus}%")
    t.add_row("[bold]SURVIVAL CHANCE[/bold]", f"[bold]{chance}%[/bold]")
    ui.console.print(ui.Panel(t, title="PRE-BREACH ASSESSMENT", border_style=ui.AMBER, box=ui.box.ROUNDED))
    if bonus_lines:
        ui.info("Loadout active: " + "  ·  ".join(bonus_lines))
    else:
        ui.bad("No loadout equipped — you go in with nothing but nerve.")

    if not ui.confirm("\nDeploy into the breach?"):
        ui.info("Deployment aborted. The cell holds — for now.")
        return

    # Survival simulation: several rounds of flavour, final resolution on the chance roll.
    ui.console.print()
    rounds = random.randint(3, 5)
    for i in range(rounds):
        line = random.choice(GENERIC_EVENTS)
        if state.loadout and random.random() < 0.6:
            item = random.choice(state.loadout)
            b = LOADOUT_BONUS.get(item)
            if b:
                line = b[1]
        ui.console.print(ui.Text(f"  ◦ {line}", style=ui.DIM))
        # tiny pacing without blocking automated tests
        ui.console.print("", end="")

    survived = random.randint(1, 100) <= chance
    ui.console.print()
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
