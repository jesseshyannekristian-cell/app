"""Generate SVG previews of the SCP Site Overseer terminal UI (rich record → SVG)."""
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from rich.console import Console
from game import ui, hub, breach
from game.state import GameState
from game.archives import Archives

OUT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "previews"))
os.makedirs(OUT, exist_ok=True)

# Non-interactive: stub out prompts/pauses so screens render once and return.
ui.pause = lambda *a, **k: None
ui.Prompt.ask = lambda *a, **k: "b"      # list screens: 'b' = back after one render
ui.Confirm.ask = lambda *a, **k: False   # breach: decline deploy (shows art+assessment)


def shot(name, render):
    rec = Console(record=True, width=104)
    ui.console = rec
    try:
        render()
    except Exception as e:  # keep generating the rest even if one screen errors
        rec.print(f"[red]preview error: {e}[/red]")
    path = os.path.join(OUT, f"{name}.svg")
    rec.save_svg(path, title=f"SCP Site Overseer — {name}")
    print("wrote", path)


def main():
    state = GameState()           # fresh save-less state
    archives = Archives()
    # Some flavour for richer previews
    state.unlocked_achievements = ["first_blood", "armed"]

    shot("01_title_briefing", lambda: (ui.title_banner(), hub.site_status(state, archives, clear=False)))
    shot("02_main_hub", lambda: (ui.header(state), ui.menu("SITE OVERSEER — MAIN HUB", [
        ("1", "Research Division"), ("2", "Requisition Store"), ("3", "Containment Operations"),
        ("4", "Pre-Breach Loadout"), ("5", "Breach Response"), ("6", "Site-20 Personnel"),
        ("7", "Ranks & Rewards"), ("8", "Archives (custom SCPs)"), ("9", "Achievements"),
        ("0", "Site Status Briefing"), ("n", "New Game / Reset"), ("q", "Save & Quit"),
    ])))
    shot("03_store", lambda: hub.store(state))
    shot("04_operations", lambda: hub.operations(state))
    shot("05_breach_select", lambda: hub.breach_menu(state, archives))
    shot("06_breach_event", lambda: breach.run_breach(
        state, {"number": "SCP-173", "name": "The Sculpture", "threat": 3}))
    shot("07_achievements", lambda: hub.achievements_menu(state))


if __name__ == "__main__":
    main()
