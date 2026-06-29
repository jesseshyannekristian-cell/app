"""FOUNDATION — SCP Site Overseer (terminal edition). Entry point.

Run:  python -m game.main      (from /app)
  or: python /app/play.py
"""
from . import ui, hub
from .state import GameState
from .archives import Archives


def main():
    state = GameState.load()
    archives = Archives.load()

    ui.title_banner()
    ui.info("  Loading Site-20 overseer terminal...")
    ui.pause()

    actions = {
        "1": lambda: hub.research_division(state),
        "2": lambda: hub.store(state),
        "3": lambda: hub.operations(state),
        "4": lambda: hub.loadout(state),
        "5": lambda: hub.breach_menu(state, archives),
        "6": lambda: hub.personnel(state),
        "7": lambda: hub.ranks(state),
        "8": lambda: hub.archives_menu(state, archives),
        "9": lambda: hub.achievements_menu(state),
        "n": lambda: hub.new_game_menu(state, archives),
    }

    try:
        while True:
            ui.clear()
            ui.header(state)
            choice = ui.menu("SITE OVERSEER — MAIN HUB", [
                ("1", "Research Division"),
                ("2", "Requisition Store"),
                ("3", "Containment Operations"),
                ("4", "Pre-Breach Loadout"),
                ("5", "Breach Response"),
                ("6", "Site-20 Personnel"),
                ("7", "Ranks & Rewards"),
                ("8", "Archives (custom SCPs)"),
                ("9", "Achievements"),
                ("n", "New Game / Reset"),
                ("q", "Save & Quit"),
            ])
            if choice == "q":
                state.save()
                archives.save()
                ui.good("Progress saved. Securing terminal. Goodbye, Overseer.")
                break
            actions[choice]()
    except (KeyboardInterrupt, EOFError):
        state.save()
        archives.save()
        ui.console.print("\n[bold #ffb000]Interrupt received — progress saved. Terminal closed.[/bold #ffb000]")


if __name__ == "__main__":
    main()
