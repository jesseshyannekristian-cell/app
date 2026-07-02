"""Shared Rich UI helpers — Foundation terminal aesthetic (amber/cyan on black)."""
import os
import time

from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.text import Text
from rich.align import Align
from rich.prompt import Prompt, Confirm
from rich import box

console = Console()

# Line-level "typewriter" pacing. Disabled (0s) when SCP_NO_DELAY is set (tests/CI).
TYPE_DELAY = 0.0 if os.environ.get("SCP_NO_DELAY") else 0.32

AMBER = "bold #ffb000"
CYAN = "bold #21d4d4"
RED = "bold #ff4b4b"
GREEN = "bold #5dff5d"
DIM = "#7a7a7a"

TITLE_ART = r"""
   _____ _____ _____    _____ _____ _______ ______
  / ____/ ____|  __ \  / ____|_   _|__   __|  ____|
 | (___| |    | |__) || (___   | |    | |  | |__
  \___ \ |    |  ___/  \___ \  | |    | |  |  __|
  ____) | |____| |      ____) |_| |_   | |  | |____
 |_____/ \_____|_|     |_____/|_____|  |_|  |______|
           O V E R S E E R   ·   S I T E - 2 0
"""


def clear():
    console.clear()


def title_banner():
    clear()
    art = Text(TITLE_ART, style=AMBER)
    sub = Text("SECURE · CONTAIN · PROTECT", style=CYAN)
    console.print(Align.center(art))
    console.print(Align.center(sub))
    console.print()


def header(state):
    rank = state.current_rank()
    bar = Table.grid(expand=True)
    bar.add_column(justify="left")
    bar.add_column(justify="right")
    left = Text.assemble(
        ("◆ RANK ", DIM), (f'{rank["title"]} (L{rank["level"]})', AMBER),
        ("   XP ", DIM), (str(state.xp), CYAN),
    )
    right = Text.assemble(
        ("CREDITS ", DIM), (str(state.credits), GREEN),
        ("   RESEARCH ", DIM), (str(state.research_credits), CYAN),
    )
    bar.add_row(left, right)
    title = "[bold]SITE OVERSEER HUB[/bold]"
    diff = getattr(state, "difficulty", "Normal")
    title += f"  [#7a7a7a]· {diff}[/#7a7a7a]"
    if getattr(state, "ironman", False):
        title += "  [bold #ff4b4b]☠ IRON-MAN[/bold #ff4b4b]"
    console.print(Panel(bar, box=box.HEAVY, border_style=AMBER, title=title))


def slow_print(text, style=CYAN, prefix="  ◦ ", delay=None):
    """Print a single log line with optional typewriter-style pacing."""
    console.print(Text(prefix + text, style=style))
    d = TYPE_DELAY if delay is None else delay
    if d:
        time.sleep(d)


def art(text, style=AMBER):
    console.print(Text(text, style=style))


def panel(body, title=None, style=CYAN):
    console.print(Panel(body, title=title, border_style=style, box=box.ROUNDED))


def info(msg):
    console.print(Text(msg, style=CYAN))


def good(msg):
    console.print(Text("✔ " + msg, style=GREEN))


def bad(msg):
    console.print(Text("✘ " + msg, style=RED))


def result(success, msg):
    (good if success else bad)(msg)


def menu(title, options):
    """options: list of (key, label). Returns the chosen key string."""
    t = Table(box=box.SIMPLE, show_header=False, padding=(0, 2))
    t.add_column(style=AMBER, justify="right")
    t.add_column()
    for key, label in options:
        t.add_row(Text(f"[{key}]", style=AMBER), label)
    console.print(Panel(t, title=title, border_style=AMBER, box=box.ROUNDED))
    valid = [k for k, _ in options]
    return Prompt.ask(Text("Select", style=AMBER), choices=valid, show_choices=False)


def ask(prompt, default=None):
    return Prompt.ask(Text(prompt, style=CYAN), default=default)


def confirm(prompt):
    return Confirm.ask(Text(prompt, style=AMBER))


def pause():
    console.input(Text("\n  Press ENTER to continue...", style=DIM))
